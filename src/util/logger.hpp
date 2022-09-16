#pragma once
#include <ktl/kformat.hpp>
#include <util/common.hpp>

namespace logger {
enum class Pipe { StdOut, StdErr };

std::string build_log_string(std::string_view text);
void do_print(Pipe pipe, char level, char const* text);
void print_verbatim(Pipe pipe, char const* text);

template <typename... Args>
void print(Pipe pipe, char level, std::string_view const fmt, Args const&... args) {
	do_print(pipe, level, build_log_string(ktl::kformat(fmt, args...)).c_str());
}

template <typename... Args>
void error(std::string_view const fmt, Args const&... args) {
	print(Pipe::StdErr, 'E', fmt, args...);
}

template <typename... Args>
void warn(std::string_view const fmt, Args const&... args) {
	print(Pipe::StdOut, 'W', fmt, args...);
}

template <typename... Args>
void info(std::string_view const fmt, Args const&... args) {
	print(Pipe::StdOut, 'I', fmt, args...);
}

template <typename... Args>
void debug(std::string_view const fmt, Args const&... args) {
	if constexpr (ROOT_NS::debug_v) { print(Pipe::StdOut, 'D', fmt, args...); }
}
} // namespace logger
