#pragma once

#include <fstream>
#include <mutex>
#include <string>

#include "logging/log_level.hpp"

class Logger
{
public:
    Logger() = delete;
    ~Logger() = delete;

    static bool init(const std::string &filePath);
    static void shutdown();

    static void set_level(LogLevel level);
    static void enable_console_output(bool enable);

    static void debug(const std::string &message);
    static void info(const std::string &message);
    static void warning(const std::string &message);
    static void error(const std::string &message);

private:
    static void log(LogLevel level, const std::string &message);
    static const char *to_string(LogLevel level);
    static std::string now_string();

private:
    static std::ofstream s_file;
    static std::mutex s_mutex;
    static LogLevel s_min_level;
    static bool s_console_enabled;
    static bool s_initialized;
};
