#include "models.h"

#include <nlohmann/json.hpp>

namespace budget {
    void to_json(nlohmann::json& j, Account const& a) {
        j = nlohmann::json{{"name", a.name}, {"balance", a.balance}};
    }

    void from_json(nlohmann::json const& j, Account& a) {
        j.at("name").get_to(a.name);
        j.at("balance").get_to(a.balance);
    }
}
