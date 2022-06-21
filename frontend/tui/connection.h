#pragma once

#include <vector>

namespace budget {
    class Account;

    auto const endpoint = "http://localhost:8000/api/";

    [[nodiscard]] std::vector<Account> get_accounts();
} // namespace budget
