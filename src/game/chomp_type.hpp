#pragma once
#include <util/enum_array.hpp>
#include <cstdint>
#include <string_view>

namespace cronch {
enum class ChompType : std::uint8_t { eNone, eFood, eDilator, eCOUNT_ };
inline constexpr util::EnumArray<ChompType, std::string_view> chomp_type_str_v = {"none", "food", "dilator"};
} // namespace cronch
