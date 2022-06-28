#include "interface.h"
#include "ftxui/dom/elements.hpp"

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
        update_current_budget_view();

        // Accounts
        auto const account_renderer = Renderer([this] { return Table(account_values_).Render(); });

        // Budget
        auto budget_menu_options = MenuOption::Vertical();
        budget_menu_options.on_change = [this] { update_current_budget_view(); };

        budget_month_menu_ =
            Menu(&budget_month_entries_, &selected_month_index_, budget_menu_options);

        auto const budget_month_menu_renderer = Renderer(budget_month_menu_, [this] {
            return window(text("Months"), budget_month_menu_->Render() | vscroll_indicator | frame);
        });

        auto const budget_renderer = Renderer([this] {
            return window(text("Budget"),
                          Table(viewed_budget_values_).Render() | vscroll_indicator | frame | flex);
        });

        auto const budget_container = Container::Horizontal(
            {budget_month_menu_renderer, budget_renderer});

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

    void Interface::update_current_budget_view() {
        spdlog::info("Updating current budget view");
        viewed_budget_values_.clear();
        auto const& budget = state_->budget(static_cast<std::size_t>(selected_month_index_));
        std::transform(budget.entries.cbegin(),
                       budget.entries.cend(),
                       std::back_inserter(viewed_budget_values_),
                       [](auto const& e) {
                           return std::vector<std::string>{
                               e.first,
                               fmt::format("{:.2f}", e.second.allocated),
                               fmt::format("{:.2f}", e.second.activity),
                               fmt::format("{:.2f}", e.second.balance)};
                       });
    }

    void Interface::loop() {
        spdlog::info("Starting main loop");
        screen_.Loop(main_container_);
    }
} // namespace budget
