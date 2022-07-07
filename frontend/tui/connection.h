#pragma once

#include "models.h"

#include <string_view>
#include <vector>

namespace cpr {
    class Response;
}

namespace budget {
    class Account;
    class BudgetMonth;

    constexpr auto endpoint = std::string_view{"http://localhost:8000/api/"};

    class HttpError : std::runtime_error {
    public:
        explicit HttpError(cpr::Response const& r);
    };

    [[nodiscard]] std::vector<Account> get_all_accounts();
    [[nodiscard]] std::vector<BudgetMonth> get_budget_months();

    template <typename T>
    void refresh_object(T& o);

    class State {
    private:
        std::vector<Account> accounts_;
        std::vector<BudgetMonth> budget_;

    public:
        [[nodiscard]] std::vector<Account> const& accounts();
        [[nodiscard]] std::vector<std::pair<int, int>> budget_months();
        [[nodiscard]] BudgetMonth const& budget(std::vector<BudgetMonth>::size_type i);
    };
} // namespace budget
