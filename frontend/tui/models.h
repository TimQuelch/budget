#pragma once

#include <compare>
#include <map>
#include <stdexcept>
#include <optional>
#include <string>
#include <type_traits>

#include <nlohmann/json_fwd.hpp>

namespace budget {

    class JsonParseError : std::runtime_error {
    public:
        explicit JsonParseError(std::string const& what);
        explicit JsonParseError(char const* what);
    };

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

        std::pair<int, int> month;
        std::string url;
        std::map<std::string, Entry> entries;

        friend std::strong_ordering operator<=>(BudgetMonth const& a, BudgetMonth const& b);
    };

    static_assert(std::is_default_constructible<BudgetMonth>::value);

    void to_json(nlohmann::json& j, BudgetMonth const& m);
    void from_json(nlohmann::json const& j, BudgetMonth& m);

    class Transaction {
    public:
        std::tuple<int, int, int> date;
        std::string url;
        std::optional<std::string> src;
        std::optional<std::string> dst;
        std::optional<std::string> category;
        std::optional<std::string> memo;
        double amount;

        friend auto operator<=>(Transaction const& a, Transaction const& b) = default;
    };

    static_assert(std::is_default_constructible<BudgetMonth>::value);

    void to_json(nlohmann::json& j, Transaction const& m);
    void from_json(nlohmann::json const& j, Transaction& m);
} // namespace budget
