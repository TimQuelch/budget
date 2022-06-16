#pragma once

#include <string>
#include <vector>

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>

namespace budget {
    class Interface {
        std::vector<std::string> tab_entries_ = {"budget", "accounts", "transactions"};
        int tab_index_ = 0;
        std::vector<std::vector<std::string>> account_values_;

        ftxui::ScreenInteractive screen_ = ftxui::ScreenInteractive::Fullscreen();
        ftxui::Component  main_container_;

    public:
        Interface();

        void update_accounts();
        void loop();
    };
} // namespace budget
