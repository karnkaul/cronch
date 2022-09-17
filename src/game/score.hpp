#pragma once
#include <cstdint>

namespace cronch {
struct Score {
	std::int64_t value{};
	std::int64_t muliplier{1};

	std::int64_t add() {
		value += muliplier;
		return muliplier++;
	}

	void reset_multiplier() { muliplier = 1; }
};
} // namespace cronch
