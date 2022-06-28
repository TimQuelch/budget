#pragma once

#include "connection.h"
#include "models.h"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <string>
#include <vector>

namespace budget {

    class Interface {
    private:
        std::unique_ptr<State> state_;

        std::vector<std::string> tab_entries_ = {"budget", "accounts", "transactions"};
        int tab_index_ = 0;
        std::vector<std::vector<std::string>> account_values_;
        ftxui::Component budget_month_menu_;
        std::vector<std::string> budget_month_entries_;
        int selected_month_index_ = 0;
        std::vector<std::vector<std::string>> viewed_budget_values_;

        ftxui::ScreenInteractive screen_ = ftxui::ScreenInteractive::Fullscreen();
        ftxui::Component main_container_;

    public:
        Interface();

        void update_accounts();
        void update_budget_months();
        void loop();
    };
} // namespace budget
