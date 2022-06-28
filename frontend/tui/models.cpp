#include "models.h"

#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

namespace {
    auto string_to_month_pair(std::string_view str) {
        // TODO Error handling
        auto const* const monthsep = std::find(str.begin(), str.end(), '-');
        auto const* const monthstart = monthsep + 1; // NOLINT (*pointer-arithmetic)
        auto const* const daysep = std::find(monthstart, str.end(), '-');
        auto const year = std::string(str.begin(), monthsep);
        auto const month = std::string(monthstart, daysep);
        return std::pair{std::stoi(year), std::stoi(month)};
    }
} // namespace

namespace budget {
    void to_json(nlohmann::json& j, Account const& a) {
        j = nlohmann::json{{"name", a.name}, {"balance", a.balance}};
    }

    void from_json(nlohmann::json const& j, Account& a) {
        j.at("name").get_to(a.name);
        j.at("balance").get_to(a.balance);
    }

    std::strong_ordering operator<=>(BudgetMonth const& a, BudgetMonth const& b) {
        return a.month <=> b.month;
    }

    // void to_json(nlohmann::json& j, BudgetMonth const& m) {
    //     j = nlohmann::json{{"name", m.name}, {"balance", m.balance}};
    // }

    void from_json(nlohmann::json const& j, BudgetMonth& m) {
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
    }
} // namespace budget
