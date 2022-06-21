#include "interface.h"

#include "connection.h"
#include "models.h"

#include <fmt/format.h>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/screen.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace budget {
    Interface::Interface() {
        using namespace ftxui;

        update_accounts(get_accounts());

        auto const account_renderer = Renderer([this] { return Table(account_values_).Render(); });

        auto const tab_selection = Menu(&tab_entries_, &tab_index_, MenuOption::Horizontal());
        auto const tab_content = Container::Tab({account_renderer, account_renderer}, &tab_index_);

        main_container_ = Container::Vertical({tab_selection, tab_content});
    }

    void Interface::update_accounts(std::vector<Account> const& accounts) {
        spdlog::info("Updating account view");
        account_values_.clear();
        std::transform(
            accounts.cbegin(),
            accounts.cend(),
            std::back_inserter(account_values_),
            [](auto const& a) {
                return std::vector<std::string>{a.name, fmt::format("{:.2f}", a.balance)};
            });
    }

    void Interface::loop() {
        spdlog::info("Starting main loop");
        screen_.Loop(main_container_);
    }
} // namespace budget
