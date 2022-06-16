#include <iostream>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "interface.h"

int main() {
    auto file_logger = spdlog::basic_logger_mt("log", "log.txt");
    spdlog::set_default_logger(file_logger);
    spdlog::set_level(spdlog::level::debug);

    try {
        auto interface = budget::Interface();
        interface.loop();

    } catch (std::exception const& e) {
        std::cout << "oops: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
