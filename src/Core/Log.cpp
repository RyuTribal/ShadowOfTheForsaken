#include "pch.h"
#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <filesystem>

#define SOF_HAS_CONSOLE !DIST

namespace SOF
{


    std::shared_ptr<spdlog::logger> Log::s_Logger;

    void Log::Init()
    {
        // Create "logs" directory if doesn't exist
        std::string logsDirectory = "logs";
        if (!std::filesystem::exists(logsDirectory)) std::filesystem::create_directories(logsDirectory);

        std::vector<spdlog::sink_ptr> LogSinks = {
            std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/GAME.log", true),
#if SOF_HAS_CONSOLE
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
#endif
        };

        LogSinks[0]->set_pattern("[%T] [%l] %n: %v");

#if SOF_HAS_CONSOLE
        LogSinks[1]->set_pattern("%^[%T] %n: %v%$");
#endif

        s_Logger = std::make_shared<spdlog::logger>("GAME", LogSinks.begin(), LogSinks.end());
        s_Logger->set_level(spdlog::level::trace);
    }

    void Log::Shutdown()
    {
        s_Logger.reset();
        spdlog::drop_all();
    }

}// namespace SOF
