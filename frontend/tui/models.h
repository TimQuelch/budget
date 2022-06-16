#pragma once

#include <string>
#include <type_traits>

#include <nlohmann/json_fwd.hpp>


namespace budget {

    class Payee {
    public:
        std::string name;

        Payee() noexcept = default;
        explicit Payee(std::string name) : name{std::move(name)} {}
    };

    static_assert(std::is_default_constructible<Payee>::value);

    struct Account : public Payee {
        double balance = 0.0;
    };

    static_assert(std::is_default_constructible<Account>::value);

    void to_json(nlohmann::json& j, Account const& a);
    void from_json(nlohmann::json const& j, Account& a);

} // namespace budget
