#pragma once
#include <random>

namespace cronch::util {
template <typename T>
T random_range(T const lo, T const hi) {
	using Distribution = std::conditional_t<std::integral<T>, std::uniform_int_distribution<T>, std::uniform_real_distribution<T>>;
	static auto engine = std::default_random_engine{std::random_device{}()};
	return Distribution{lo, hi}(engine);
}
} // namespace cronch::util
