#pragma once
#include <array>
#include <concepts>

namespace cronch::util {
template <typename Enum, typename Type, std::size_t Size = static_cast<std::size_t>(Enum::eCOUNT_)>
struct EnumArray {
	std::array<Type, Size> array{};

	EnumArray() = default;

	template <std::convertible_to<Type>... T>
	constexpr EnumArray(T... t) : array{std::move(t)...} {}

	constexpr Type& operator[](Enum const e) { return array[static_cast<std::size_t>(e)]; }
	constexpr Type const& operator[](Enum const e) const { return array[static_cast<std::size_t>(e)]; }
};
} // namespace cronch::util
