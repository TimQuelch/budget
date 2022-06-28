#pragma once

#include <map>
#include <string>
#include <type_traits>

#include <nlohmann/json_fwd.hpp>

namespace budget {

    class Payee {
    public:
        std::string url;
        std::string name;
    };

    static_assert(std::is_default_constructible<Payee>::value);

    class Account : public Payee {
    public:
        double balance = 0.0;
    };

    static_assert(std::is_default_constructible<Account>::value);

    void to_json(nlohmann::json& j, Account const& a);
    void from_json(nlohmann::json const& j, Account& a);

    class BudgetMonth {
    public:
        struct Entry {
            double allocated;
            double activity;
            double balance;
        };

        std::string url;
        std::pair<int, int> month;
        std::map<std::string, Entry> entries;
    };

    static_assert(std::is_default_constructible<BudgetMonth>::value);

    void to_json(nlohmann::json& j, BudgetMonth const& m);
    void from_json(nlohmann::json const& j, BudgetMonth& m);

} // namespace budget
