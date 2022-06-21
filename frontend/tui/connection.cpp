#include "connection.h"

#include "models.h"

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace {
    nlohmann::json get_json(std::string const& path) {
        auto const url = cpr::Url{budget::endpoint} + path;
        spdlog::info("Sending GET request to {}", std::string(url));
        cpr::Response r = cpr::Get(url);
        spdlog::info("Received response status {} type {} in {}s",
                     r.status_code,
                     r.header["content-type"],
                     r.elapsed);
        auto j = nlohmann::json::parse(r.text);
        spdlog::debug(j.dump(2));
        return j;
    }
} // namespace

namespace budget {
    [[nodiscard]] std::vector<Account> get_accounts() {
        return get_json("accounts/").get<std::vector<budget::Account>>();
    }
} // namespace budget
