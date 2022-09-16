#pragma once
#include <istream>
#include <string>

namespace cronch::util {
struct Property {
	std::string key{};
	std::string value{};

	explicit operator bool() const { return !key.empty(); }

	struct Parser;
};

struct Property::Parser {
	std::istream& in;

	Property next();

	explicit operator bool() const { return static_cast<bool>(in); }

	template <typename F>
	void parse_all(F&& func) {
		while (auto property = next()) { func(std::move(property)); }
	}
};
} // namespace cronch::util
