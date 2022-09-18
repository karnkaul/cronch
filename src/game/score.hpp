#pragma once
#include <cstdint>

namespace cronch {
struct Score {
	static constexpr std::int64_t max_value_v{999999999};

	std::int64_t value{};
	std::int64_t muliplier{1};
	std::int64_t unit{10};

	static constexpr std::int64_t clamp(std::int64_t score, std::int64_t multiplier, std::int64_t unit) {
		auto const ret = score + multiplier * unit;
		if (ret < 0) { return 0; }
		if (ret > max_value_v) { return max_value_v; }
		return ret;
	}

	constexpr std::int64_t add() {
		value = clamp(value, muliplier, unit);
		if (value == max_value_v) { return muliplier; }
		return muliplier++;
	}

	constexpr void reset_multiplier() { muliplier = 1; }
};
} // namespace cronch
