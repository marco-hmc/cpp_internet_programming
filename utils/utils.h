#pragma once

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <utility>

enum class Status { kSuccess, kFailure };

#define DISABLE_COPY_AND_ASSIGN(ClassName) \
    ClassName(const ClassName&) = delete;  \
    ClassName& operator=(const ClassName&) = delete;

#define DISABLE_CONSTRUCTION(ClassName)   \
    ClassName() = delete;                 \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

#define DISABLE_MOVE(ClassName)      \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

#if defined(__clang__)
#define NO_OPTIMIZE __attribute__((optnone))
#elif defined(__GNUC__) || defined(__GNUG__)
#define NO_OPTIMIZE __attribute__((optimize("O0")))
#else
#define NO_OPTIMIZE
#endif

namespace myUtils {

    inline void init_logger(const std::string& log_file = "") {
        try {
            if (!log_file.empty()) {
                auto file_logger =
                    spdlog::basic_logger_mt("file_logger", log_file);
                spdlog::set_default_logger(file_logger);
            } else {
                auto console_logger = spdlog::stdout_color_mt("console_logger");
                spdlog::set_default_logger(console_logger);
            }
            spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%^%l%$] %v");
            spdlog::info("Logger initialized successfully.");
        } catch (const spdlog::spdlog_ex& ex) {
            std::cerr << "Log initialization failed: " << ex.what()
                      << std::endl;
        }
    }

    template <typename Func, typename... Args>
    inline void measure_time_console(Func&& func, Args&&... args) {
        auto start = std::chrono::high_resolution_clock::now();
        std::forward<Func>(func)(std::forward<Args>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        double milliseconds = duration.count() / 1e6;
        if (milliseconds < 1000) {
            std::cout << std::scientific << std::setprecision(2) << milliseconds
                      << " ms" << std::endl;
        } else {
            std::cout << std::scientific << std::setprecision(2)
                      << milliseconds / 1000 << " s" << std::endl;
        }
    }

    template <typename Func, typename... Args>
    inline void measure_time(Func&& func, Args&&... args) {
        auto start = std::chrono::high_resolution_clock::now();
        std::forward<Func>(func)(std::forward<Args>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        double milliseconds = duration.count() / 1e6;
        if (milliseconds < 1000) {
            spdlog::info("\t{:.2e} ms", milliseconds);
        } else {
            spdlog::info("\t{:.2e} s", milliseconds / 1000);
        }
    }

}  // namespace myUtils