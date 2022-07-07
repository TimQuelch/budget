#include "connection.h"

#include "models.h"

#include <algorithm>
#include <exception>
#include <iterator>
#include <stdexcept>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

namespace {
    nlohmann::json get_json(std::string_view url, cpr::Parameters const& params) {
        spdlog::info("Sending GET request to {}", url);
        cpr::Response r = cpr::Get(cpr::Url{std::string{url}}, params);
        if (r.status_code == 0 || r.status_code >= 400) { // NOLINT (*magic-numbers)
            throw budget::HttpError(r);
        }
        spdlog::info("Received response status {} type {} in {}s",
                     r.status_code,
                     r.header["content-type"],
                     r.elapsed);
        auto j = nlohmann::json::parse(r.text);
        spdlog::debug(j.dump(2));
        return j;
    }

    nlohmann::json get_json(std::string_view url) { return get_json(url, {}); }

    void check_valid_url_or_throw(std::string_view url) {
        if (!url.starts_with(budget::endpoint)) {
            throw std::invalid_argument("Bad url");
        }
    }

    std::string response_error_string(cpr::Response const& r) {
        if (r.status_code == 0) {
            return fmt::format("Request error: {}", r.error.message);
        }
        return fmt::format("HTTP status {}: {}", r.status_code, r.status_line);
    }
} // namespace

namespace budget {
    HttpError::HttpError(cpr::Response const& r) : std::runtime_error{response_error_string(r)} {
        spdlog::error(what());
    }

    [[nodiscard]] std::vector<Account> get_all_accounts() {
        return get_json(std::string{endpoint} + "accounts/").get<std::vector<Account>>();
    }

    [[nodiscard]] std::vector<BudgetMonth> get_budget_months() {
        auto vec = get_json(std::string{endpoint} + "budget/").get<std::vector<BudgetMonth>>();
        std::sort(vec.begin(), vec.end(), std::greater<>());
        return vec;
    }

    template <typename T>
    void refresh_object(T& o) {
        check_valid_url_or_throw(o.url);
        get_json(o.url).get_to(o);
    }

    // Manually instantiate template for types which I actually want
    template void refresh_object<Account>(Account&);
    template void refresh_object<BudgetMonth>(BudgetMonth&);
    template void refresh_object<Transaction>(Transaction&);

    [[nodiscard]] std::vector<Transaction> const& Ledger::transactions() {
        if (transactions_.empty()) {
            auto const j = get_json(std::string{endpoint} + "ledger/",
                                    cpr::Parameters{{"limit", std::to_string(page_limit)}});
            j.at("count").get_to(count_);
            if (!j.at("next").is_null()) {
                j["next"].get_to(next_url_);
            }
            // There shouldn't be a previous URL (as this should start from the first page)
            j.at("results").get_to(transactions_);

            if (!next_url_ && count_ != transactions_.size()) {
                spdlog::warn("JSON count ({}) doesn't match the number of retrieved transactions "
                             "({}) (when next url is present)",
                             count_,
                             transactions_.size());
            }
        }

        return transactions_;
    }

    void Ledger::load_next() {
        if (next_url_) {
            check_valid_url_or_throw(*next_url_);
            auto const j = get_json(*next_url_);

            // Assign urls (or remove if missing)
            if (j.at("next").is_null()) {
                next_url_.reset();
            } else {
                j["next"].get_to(next_url_);
            }
            if (j.at("previous").is_null()) {
                prev_url_.reset();
            } else {
                j["previous"].get_to(prev_url_);
            }

            auto const newcount = j.at("count").get<std::size_t>();
            if (newcount != count_) {
                count_ = newcount;
                spdlog::warn("Count has changed between paginated calls");
            }

            auto newtransactions = j.at("results").get<std::vector<Transaction>>();
            transactions_.reserve(transactions_.size() + newtransactions.size());
            transactions_.insert(transactions_.end(),
                                 std::make_move_iterator(newtransactions.begin()),
                                 std::make_move_iterator(newtransactions.end()));
        }
    }

    void Ledger::load_all() {
        while (next_url_) {
            load_next();
        }
    }

    [[nodiscard]] std::vector<Account> const& State::accounts() {
        if (accounts_.empty()) {
            accounts_ = get_all_accounts();
        }
        return accounts_;
    }

    [[nodiscard]] std::vector<std::pair<int, int>> State::budget_months() {
        if (budget_.empty()) {
            budget_ = get_budget_months();
        }
        auto ret = std::vector<std::pair<int, int>>{};
        std::transform(budget_.cbegin(),
                       budget_.cend(),
                       std::back_inserter(ret),
                       [](auto const& bm) { return bm.month; });
        return ret;
    };

    [[nodiscard]] BudgetMonth const& State::budget(std::vector<BudgetMonth>::size_type i) {
        if (budget_.at(i).entries.empty()) {
            refresh_object(budget_.at(i));
        }
        return budget_.at(i);
    }
} // namespace budget
