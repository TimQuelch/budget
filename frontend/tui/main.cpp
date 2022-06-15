#include <algorithm>
#include <iostream>
#include <iterator>

#include <nlohmann/json.hpp>

#include <fmt/format.h>

#include <cpr/cpr.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/screen.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

auto const endpoint = "http://localhost:8000/api/";

json get_json(std::string const& path) {
    auto const url = cpr::Url{endpoint} + path;
    spdlog::info("Sending GET request to {}", std::string(url));
    cpr::Response r = cpr::Get(url);
    spdlog::info("Received response status {} type {} in {}s", r.status_code, r.header["content-type"], r.elapsed);
    auto j = json::parse(r.text);
    spdlog::debug(j.dump(2));
    return j;
}

json get_accounts() { return get_json("accounts/"); }

int main() {
    auto file_logger = spdlog::basic_logger_mt("log", "log.txt");
    spdlog::set_default_logger(file_logger);
    spdlog::set_level(spdlog::level::debug);

    using namespace ftxui;

    try {
        auto const account_values = []{
            auto const account_json = get_accounts();
            auto ret = std::vector<std::vector<std::string>>();
            std::transform(account_json.cbegin(), account_json.cend(), std::back_inserter(ret), [](json const& a) {
                return std::vector<std::string>{a["name"].get<std::string>(), fmt::format("{:.2f}", a["balance"].get<double>())};
            });
            return ret;
        }();

        auto const accounts = Renderer(
            [&account_values] { return Table(account_values).Render(); });

        auto const tab_entries = std::vector<std::string>{"accounts", "transactions"};
        int tab_index = 0;
        auto const tab_selection = Menu(&tab_entries, &tab_index, MenuOption::Horizontal());
        auto const tab_content = Container::Tab({accounts, accounts}, &tab_index);

        auto const main_container = Container::Vertical({tab_selection, tab_content});

        auto screen = ScreenInteractive::Fullscreen();

        screen.Loop(main_container);

    } catch (std::exception const& e) {
        std::cout << "oops: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
