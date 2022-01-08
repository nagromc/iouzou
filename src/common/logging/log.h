// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <algorithm>
#include <string_view>
#include <type_traits>

#include <fmt/format.h>

#include "common/logging/types.h"

// adapted from https://github.com/fmtlib/fmt/issues/2704
// a generic formatter for enum classes (<= 32 bits)
#if FMT_VERSION >= 80100
template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_enum_v<T>, char>>
    : formatter<std::underlying_type_t<T>> {
    template <typename FormatContext>
    auto format(const T& value, FormatContext& ctx) -> decltype(ctx.out()) {
        return fmt::formatter<std::underlying_type_t<T>>::format(
            static_cast<std::underlying_type_t<T>>(value), ctx);
    }
};
#endif

namespace Common::Log {

// trims up to and including the last of ../, ..\, src/, src\ in a string
constexpr const char* TrimSourcePath(std::string_view source) {
    const auto rfind = [source](const std::string_view match) {
        return source.rfind(match) == source.npos ? 0 : (source.rfind(match) + match.size());
    };
    auto idx = std::max({rfind("src/"), rfind("src\\"), rfind("../"), rfind("..\\")});
    return source.data() + idx;
}

/// Logs a message to the global logger, using fmt
void FmtLogMessageImpl(Class log_class, Level log_level, const char* filename,
                       unsigned int line_num, const char* function, const char* format,
                       const fmt::format_args& args);

template <typename... Args>
void FmtLogMessage(Class log_class, Level log_level, const char* filename, unsigned int line_num,
                   const char* function, const char* format, const Args&... args) {
    FmtLogMessageImpl(log_class, log_level, filename, line_num, function, format,
                      fmt::make_format_args(args...));
}

} // namespace Common::Log

#ifdef _DEBUG
#define LOG_TRACE(log_class, ...)                                                                  \
    Common::Log::FmtLogMessage(Common::Log::Class::log_class, Common::Log::Level::Trace,           \
                               Common::Log::TrimSourcePath(__FILE__), __LINE__, __func__,          \
                               __VA_ARGS__)
#else
#define LOG_TRACE(log_class, fmt, ...) (void(0))
#endif

#define LOG_DEBUG(log_class, ...)                                                                  \
    Common::Log::FmtLogMessage(Common::Log::Class::log_class, Common::Log::Level::Debug,           \
                               Common::Log::TrimSourcePath(__FILE__), __LINE__, __func__,          \
                               __VA_ARGS__)
#define LOG_INFO(log_class, ...)                                                                   \
    Common::Log::FmtLogMessage(Common::Log::Class::log_class, Common::Log::Level::Info,            \
                               Common::Log::TrimSourcePath(__FILE__), __LINE__, __func__,          \
                               __VA_ARGS__)
#define LOG_WARNING(log_class, ...)                                                                \
    Common::Log::FmtLogMessage(Common::Log::Class::log_class, Common::Log::Level::Warning,         \
                               Common::Log::TrimSourcePath(__FILE__), __LINE__, __func__,          \
                               __VA_ARGS__)
#define LOG_ERROR(log_class, ...)                                                                  \
    Common::Log::FmtLogMessage(Common::Log::Class::log_class, Common::Log::Level::Error,           \
                               Common::Log::TrimSourcePath(__FILE__), __LINE__, __func__,          \
                               __VA_ARGS__)
#define LOG_CRITICAL(log_class, ...)                                                               \
    Common::Log::FmtLogMessage(Common::Log::Class::log_class, Common::Log::Level::Critical,        \
                               Common::Log::TrimSourcePath(__FILE__), __LINE__, __func__,          \
                               __VA_ARGS__)
