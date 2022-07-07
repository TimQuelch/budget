#pragma once

#include "models.h"

#include <optional>
#include <string_view>
#include <vector>

namespace cpr {
    class Response;
}

namespace budget {
    class Account;
    class BudgetMonth;
    class Transaction;

    constexpr auto endpoint = std::string_view{"http://localhost:8000/api/"};

    class HttpError : std::runtime_error {
    public:
        explicit HttpError(cpr::Response const& r);
    };

    [[nodiscard]] std::vector<Account> get_all_accounts();
    [[nodiscard]] std::vector<BudgetMonth> get_budget_months();
    [[nodiscard]] std::vector<Account> get_transactions();

    template <typename T>
    void refresh_object(T& o);

    class Ledger {
    private:
        std::vector<Transaction> transactions_;
        std::optional<std::string> next_url_;
        std::optional<std::string> prev_url_;
        std::size_t count_;

        constexpr static int page_limit = 100;

    public:
        [[nodiscard]] std::vector<Transaction> const& transactions();

        void load_next();
        void load_all();
    };

    class State {
    private:
        std::vector<Account> accounts_;
        std::vector<BudgetMonth> budget_;
        Ledger ledger_;

    public:
        [[nodiscard]] std::vector<Account> const& accounts();
        [[nodiscard]] std::vector<std::pair<int, int>> budget_months();
        [[nodiscard]] BudgetMonth const& budget(std::vector<BudgetMonth>::size_type i);
        [[nodiscard]] std::vector<Transaction> const& transactions();
    };
} // namespace budget
