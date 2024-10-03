#pragma once

#include "Engine/Core/Base.h"
#include "LogCustomFormatters.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#include <map>

namespace SOF
{

    class Log
    {
        public:
        enum class Level : uint8_t { Trace = 0, Info, Warn, Error, Fatal };
        struct TagDetails
        {
            bool Enabled = true;
            Level LevelFilter = Level::Trace;
        };

        public:
        static void Init();
        static void Shutdown();

        inline static std::shared_ptr<spdlog::logger> &GetLogger() { return s_Logger; }

        static bool HasTag(const std::string &tag) { return s_EnabledTags.find(tag) != s_EnabledTags.end(); }
        static std::map<std::string, TagDetails> &EnabledTags() { return s_EnabledTags; }

        template<typename... Args> static void PrintMessage(Log::Level level, std::string_view tag, Args &&...args);

        template<typename... Args> static void PrintAssertMessage(std::string_view prefix, Args &&...args);

        public:
        // Enum utils
        static const char *LevelToString(Level level)
        {
            switch (level) {
            case Level::Trace:
                return "Trace";
            case Level::Info:
                return "Info";
            case Level::Warn:
                return "Warn";
            case Level::Error:
                return "Error";
            case Level::Fatal:
                return "Fatal";
            }
            return "";
        }
        static Level LevelFromString(std::string_view string)
        {
            // Before I hear another "But can't you just..." from Elliot, no! Can't do switch statements on strings
            if (string == "Trace") return Level::Trace;
            if (string == "Info") return Level::Info;
            if (string == "Warn") return Level::Warn;
            if (string == "Error") return Level::Error;
            if (string == "Fatal") return Level::Fatal;

            return Level::Trace;
        }

        private:
        static std::shared_ptr<spdlog::logger> s_Logger;

        inline static std::map<std::string, TagDetails> s_EnabledTags;
    };

}// namespace SOF

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tagged logs (prefer these!)                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SOF_TRACE(tag, ...) ::SOF::Log::PrintMessage(::SOF::Log::Level::Trace, tag, __VA_ARGS__)
#define SOF_INFO(tag, ...) ::SOF::Log::PrintMessage(::SOF::Log::Level::Info, tag, __VA_ARGS__)
#define SOF_WARN(tag, ...) ::SOF::Log::PrintMessage(::SOF::Log::Level::Warn, tag, __VA_ARGS__)
#define SOF_ERROR(tag, ...) ::SOF::Log::PrintMessage(::SOF::Log::Level::Error, tag, __VA_ARGS__)
#define SOF_FATAL(tag, ...) ::SOF::Log::PrintMessage(::SOF::Log::Level::Fatal, tag, __VA_ARGS__)

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SOF_TRACE_NOTAG(...) ::SOF::Log::PrintMessage(::SOF::Log::Level::Trace, "", __VA_ARGS__)
#define SOF_INFO_NOTAG(...) ::SOF::Log::PrintMessage(::SOF::Log::Level::Info, "", __VA_ARGS__)
#define SOF_WARN_NOTAG(...) ::SOF::Log::PrintMessage(::SOF::Log::Level::Warn, "", __VA_ARGS__)
#define SOF_ERROR_NOTAG(...) ::SOF::Log::PrintMessage(::SOF::Log::Level::Error, "", __VA_ARGS__)
#define SOF_FATAL_NOTAG(...) ::SOF::Log::PrintMessage(::SOF::Log::Level::Fatal, "", __VA_ARGS__)

namespace SOF
{

    template<typename... Args> void Log::PrintMessage(Log::Level level, std::string_view tag, Args &&...args)
    {
        auto detail = s_EnabledTags[std::string(tag)];
        if (detail.Enabled && detail.LevelFilter <= level) {
            auto logger = GetLogger();
            std::string logString = tag.empty() ? "{0}{1}" : "[{0}] {1}";
            switch (level) {
            case Level::Trace:
                logger->trace(logString, tag, fmt::format(std::forward<Args>(args)...));
                break;
            case Level::Info:
                logger->info(logString, tag, fmt::format(std::forward<Args>(args)...));
                break;
            case Level::Warn:
                logger->warn(logString, tag, fmt::format(std::forward<Args>(args)...));
                break;
            case Level::Error:
                logger->error(logString, tag, fmt::format(std::forward<Args>(args)...));
                break;
            case Level::Fatal:
                logger->critical(logString, tag, fmt::format(std::forward<Args>(args)...));
                break;
            }
        }
    }


    template<typename... Args> void Log::PrintAssertMessage(std::string_view prefix, Args &&...args)
    {
        auto logger = GetLogger();
        logger->error("{0}: {1}", prefix, fmt::format(std::forward<Args>(args)...));
    }

    template<> inline void Log::PrintAssertMessage(std::string_view prefix)
    {
        auto logger = GetLogger();
        logger->error("{0}", prefix);
    }
}// namespace SOF
