#include "interface.h"

#include "models.h"

#include <cpr/cpr.h>
#include <fmt/format.h>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/screen.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace {

    auto const endpoint = "http://localhost:8000/api/";

    nlohmann::json get_json(std::string const& path) {
        auto const url = cpr::Url{endpoint} + path;
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

    auto get_accounts() { return get_json("accounts/").get<std::vector<budget::Account>>(); }
} // namespace

namespace budget {
    Interface::Interface() {
        using namespace ftxui;

        update_accounts();

        auto const account_renderer = Renderer([this] { return Table(account_values_).Render(); });

        auto const tab_selection = Menu(&tab_entries_, &tab_index_, MenuOption::Horizontal());
        auto const tab_content = Container::Tab({account_renderer, account_renderer}, &tab_index_);

        main_container_ = Container::Vertical({tab_selection, tab_content});
    }

    void Interface::update_accounts() {
        auto const account_json = get_accounts();
        account_values_.clear();
        std::transform(
            account_json.cbegin(),
            account_json.cend(),
            std::back_inserter(account_values_),
            [](auto const& a) {
                return std::vector<std::string>{a.name, fmt::format("{:.2f}", a.balance)};
            });
    }

    void Interface::loop() {
        screen_.Loop(main_container_);
    }
} // namespace budget
