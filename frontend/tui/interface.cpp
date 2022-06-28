#include "interface.h"

#include <fmt/format.h>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/screen.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace budget {
    Interface::Interface() {
        using namespace ftxui;

        state_ = std::make_unique<State>();

        update_accounts();
        update_budget_months();

        // Accounts
        auto const account_renderer = Renderer([this] { return Table(account_values_).Render(); });

        // Budget
        budget_month_menu_ =
            Menu(&budget_month_entries_, &selected_month_index_, MenuOption::Vertical());
        auto const budget_month_menu_renderer = Renderer(budget_month_menu_, [this] {
            return budget_month_menu_->Render() | frame;
        });
        auto const budget_container = Container::Horizontal({budget_month_menu_renderer});

        // Tabbed menu
        auto const tab_selection = Menu(&tab_entries_, &tab_index_, MenuOption::Horizontal());
        auto const tab_content = Container::Tab({budget_container, account_renderer}, &tab_index_);

        main_container_ = Container::Vertical({tab_selection, tab_content});
    }

    void Interface::update_accounts() {
        spdlog::info("Updating account view");
        account_values_.clear();
        auto const& accounts = state_->accounts();
        std::transform(
            accounts.cbegin(),
            accounts.cend(),
            std::back_inserter(account_values_),
            [](auto const& a) {
                return std::vector<std::string>{a.name, fmt::format("{:.2f}", a.balance)};
            });
    }

    void Interface::update_budget_months() {
        spdlog::info("Updating budget view");
        budget_month_entries_.clear();
        auto const& bms = state_->budget_months();
        std::transform(
            bms.cbegin(),
            bms.cend(),
            std::back_inserter(budget_month_entries_),
            [](auto const& m) { return std::string(fmt::format("{}-{:02}", m.first, m.second)); });
    }

    void Interface::loop() {
        spdlog::info("Starting main loop");
        screen_.Loop(main_container_);
    }
} // namespace budget
