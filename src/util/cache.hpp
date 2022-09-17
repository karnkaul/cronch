#pragma once
#include <concepts>

namespace cronch {
template <std::equality_comparable Type>
struct Cache {
	Type value{};

	constexpr bool changed(Type new_value) {
		if (value != new_value) {
			value = static_cast<Type&&>(new_value);
			return true;
		}
		return false;
	}
};
} // namespace cronch
