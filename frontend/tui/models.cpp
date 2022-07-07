#include "models.h"

#include <string_view>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace {
    auto string_to_month_pair(std::string_view str) {
        auto const* const monthsep = std::find(str.begin(), str.end(), '-');
        if (monthsep == str.end()) {
            throw budget::JsonParseError(fmt::format("Date format incorrect ({})", str));
        }
        auto const* const monthstart = monthsep + 1; // NOLINT (*pointer-arithmetic)
        auto const* const daysep = std::find(monthstart, str.end(), '-');
        if (daysep == str.end()) {
            throw budget::JsonParseError(fmt::format("Date format incorrect ({})", str));
        }
        auto const year = std::string(str.begin(), monthsep);
        auto const month = std::string(monthstart, daysep);
        return std::pair{std::stoi(year), std::stoi(month)};
    }

    auto string_to_ymd_tuple(std::string_view str) {
        auto const* const monthsep = std::find(str.begin(), str.end(), '-');
        if (monthsep == str.end()) {
            throw budget::JsonParseError(fmt::format("Date format incorrect ({})", str));
        }
        auto const* const monthstart = monthsep + 1; // NOLINT (*pointer-arithmetic)
        auto const* const daysep = std::find(monthstart, str.end(), '-');
        if (daysep == str.end()) {
            throw budget::JsonParseError(fmt::format("Date format incorrect ({})", str));
        }
        auto const* const daystart = daysep + 1; // NOLINT (*pointer-arithmetic)
        auto const year = std::string(str.begin(), monthsep);
        auto const month = std::string(monthstart, daysep);
        auto const day = std::string(daystart, str.end());
        return std::tuple{std::stoi(year), std::stoi(month), std::stoi(day)};
    }
} // namespace

namespace budget {

    JsonParseError::JsonParseError(std::string const& what) : std::runtime_error(what) {
        spdlog::error("Failed to parse JSON object: {}", what);
    }
    JsonParseError::JsonParseError(char const* what) : JsonParseError(std::string(what)) {}

    void to_json(nlohmann::json& j, Account const& a) {
        j = nlohmann::json{{"name", a.name}, {"balance", a.balance}};
    }

    void from_json(nlohmann::json const& j, Account& a) {
        try {
            j.at("name").get_to(a.name);
            j.at("balance").get_to(a.balance);
        } catch (nlohmann::json::exception const& e) {
            throw JsonParseError(
                fmt::format("Failed to parse account json ({}): {}", e.what(), j.dump()));
        }
    }

    std::strong_ordering operator<=>(BudgetMonth const& a, BudgetMonth const& b) {
        return a.month <=> b.month;
    }

    // void to_json(nlohmann::json& j, BudgetMonth const& m) {
    //     j = nlohmann::json{{"name", m.name}, {"balance", m.balance}};
    // }

    void from_json(nlohmann::json const& j, BudgetMonth& m) {
        try {
            j.at("url").get_to(m.url);
            m.month = string_to_month_pair(j.at("month").get<std::string>());

            if (j.contains("entries")) {
                for (auto const& e : j["entries"]) {
                    auto entry = BudgetMonth::Entry{
                        .allocated = e.at("allocated").get<double>(),
                        .activity = e.at("activity").get<double>(),
                        .balance = e.at("balance").get<double>(),
                    };
                    m.entries.insert_or_assign(e.at("category").get<std::string>(), entry);
                }
            }
        } catch (nlohmann::json::exception const& e) {
            throw JsonParseError(
                fmt::format("Failed to parse budget json ({}): {}", e.what(), j.dump()));
        }
    }

    // void to_json(nlohmann::json& j, Transaction const& m) {
    // }

    void from_json(nlohmann::json const& j, Transaction& m) {
        try {
            j.at("url").get_to(m.url);
            j.at("src").get_to(m.src);
            j.at("dst").get_to(m.dst);
            j.at("category").get_to(m.category);
            j.at("memo").get_to(m.memo);
            j.at("amount").get_to(m.amount);
            m.date = string_to_ymd_tuple(j.at("date").get<std::string>());
        } catch (nlohmann::json::exception const& e) {
            throw JsonParseError(
                fmt::format("Failed to parse transaction json ({}): {}", e.what(), j.dump()));
        }
    }
} // namespace budget
