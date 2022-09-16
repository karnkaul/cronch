#include <util/property.hpp>

namespace cronch::util {
namespace {
std::string trim(std::string_view str) {
	while (!str.empty() && std::isspace(static_cast<unsigned char>(str.front()))) { str = str.substr(1); }
	while (!str.empty() && std::isspace(static_cast<unsigned char>(str.back()))) { str = str.substr(0, str.size() - 1); }
	return std::string(str);
}

bool make_prop(Property& out, std::string_view const line) {
	if (line.empty() || line.front() == '#') { return false; }
	auto const eq = line.find('=');
	if (eq == std::string_view::npos) {
		out = {trim(line)};
		return true;
	}
	out = {trim(line.substr(0, eq)), trim(line.substr(eq + 1))};
	return true;
}
} // namespace

Property Property::Parser::next() {
	while (in) {
		auto ret = Property{};
		auto line = std::string{};
		std::getline(in, line);
		if (make_prop(ret, trim(line))) { return ret; }
	}
	return {};
}
} // namespace cronch::util
