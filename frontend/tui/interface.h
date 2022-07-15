#pragma once

#include "connection.h"
#include "ftxui/component/component_base.hpp"
#include "models.h"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <string>
#include <vector>

namespace budget {

    // Simple wrapper class for bool. This is so that I can use std::vector<Bool> without the
    // std::vector<bool> insane default bitpacking. In the default specialisation of
    // std::vector<bool>, you can't take the address of individual elements, which e.g. is required
    // when creating lists of checkbox values
    struct Bool {
    public:
        bool val;
        Bool(bool b) noexcept : val{b} {}
    };

    class Interface {
    private:
        std::unique_ptr<State> state_;

        std::vector<std::string> tab_entries_ = {"budget", "accounts", "transactions"};
        int tab_index_ = 0;
        std::vector<std::vector<std::string>> account_values_;

        ftxui::Component budget_month_menu_;
        std::vector<std::string> budget_month_entries_;
        int selected_month_index_ = 0;
        ftxui::Component budget_entry_menu_;
        std::vector<std::string> budget_entry_entries_;
        int selected_entry_index_ = 0;
        int budget_menu_global_index_ = 0;

        ftxui::Component ledger_container_;
        std::vector<Bool> transaction_selected_;

        static constexpr auto default_date_width = 12;
        static constexpr auto default_src_width = 12;
        static constexpr auto default_dst_width = 12;
        static constexpr auto default_category_width = 12;
        static constexpr auto default_memo_width = 12;
        static constexpr auto default_amount_width = 12;

        int date_width_ = default_date_width;
        int src_width_ = default_src_width;
        int dst_width_ = default_dst_width;
        int category_width_ = default_category_width;
        int memo_width_ = default_memo_width;
        int amount_width_ = default_amount_width;

        ftxui::ScreenInteractive screen_ = ftxui::ScreenInteractive::Fullscreen();
        ftxui::Component main_container_;

        void update_ledger_widths();
        ftxui::Component transaction_component(Transaction const& t, Bool* b) const;

    public:
        Interface();

        void update_accounts();
        void update_budget_months();
        void update_current_budget_view();
        void update_ledger_view();
        void loop();
    };
} // namespace budget
