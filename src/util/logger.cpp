#include <util/logger.hpp>
#include <ctime>

namespace {
std::string timestamp() {
	auto const now = std::time(nullptr);
	auto ret = std::string(16, '\0');
	ret[0] = '[';
	auto const written = std::strftime(ret.data() + 1, ret.size() - 2, "%T", std::localtime(&now));
	if (written == 0) { return {}; }
	ret[written + 1] = ']';
	return ret;
}
} // namespace

std::string logger::build_log_string(std::string_view const text) { return ktl::kformat("{} {}", text, timestamp()); }

void logger::do_print(Pipe pipe, char level, char const* text) {
	auto const out = pipe == Pipe::StdErr ? stderr : stdout;
	std::fprintf(out, "[%c] %s\n", level, text);
}

void logger::print_verbatim(Pipe pipe, char const* text) {
	auto const out = pipe == Pipe::StdErr ? stderr : stdout;
	std::fprintf(out, "%s\n", text);
}
