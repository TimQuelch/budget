#include "interface.h"

#include <algorithm>

#include <fmt/format.h>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/screen.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace {
    auto currency_element(double val) {
        using namespace ftxui;
        constexpr auto currency_width = 12;
        auto t = text(fmt::format("{}{:.2f}", val >= 0 ? " $" : "-$", std::abs(val)));
        return align_right(t) | size(WIDTH, EQUAL, currency_width);
    }

    auto balance_element(double val) {
        using namespace ftxui;
        auto el = currency_element(val);
        if (val >= 0) {
            el |= color(Color::Green);
        }
        if (val <= 0) {
            el |= color(Color::Red);
        }
        return el;
    }
} // namespace

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

        auto budget_entry_options = MenuOption::Vertical();

        budget_entry_options.entries.transform = [this](EntryState const& s) {
            auto const& budget = state_->budget(static_cast<std::size_t>(selected_month_index_));
            // TODO Fix this. It is broken
            auto const max_width = static_cast<int>(
                std::max_element(budget.entries.cbegin(),
                                 budget.entries.cend(),
                                 [](auto const& a, auto const& b) {
                                     return a.first.length() < b.first.length();
                                 })
                    ->first.length());
            spdlog::debug("Calculating max width of budget categories as {}", max_width);
            auto const vals = budget.entries.at(s.label);
            Element e = hbox({text(s.label) | size(WIDTH, EQUAL, max_width + 2),
                              currency_element(vals.allocated),
                              currency_element(vals.activity),
                              balance_element(vals.balance)});

            if (s.focused) {
                e |= inverted;
            }
            if (s.active) {
                e |= bold;
            }
            if (!s.focused && !s.active) {
                e |= dim;
            }

            return e;
        };

        budget_entry_menu_ =
            Menu(&budget_entry_entries_, &selected_entry_index_, budget_entry_options);

        auto const budget_renderer = Renderer(budget_entry_menu_, [this] {
            return window(text("Budget"),
                          budget_entry_menu_->Render() | vscroll_indicator | frame | flex);
        });

        auto const budget_container = Container::Horizontal(
            {budget_month_menu_renderer, budget_renderer},
            &budget_menu_global_index_);

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
        budget_entry_entries_.clear();
        auto const& budget = state_->budget(static_cast<std::size_t>(selected_month_index_));
        std::transform(budget.entries.cbegin(),
                       budget.entries.cend(),
                       std::back_inserter(budget_entry_entries_),
                       [](auto const& e) { return e.first; });
    }

    void Interface::loop() {
        spdlog::info("Starting main loop");
        screen_.Loop(main_container_);
    }
} // namespace budget
