#pragma once
#include <glm/vec2.hpp>
#include <util/enum_array.hpp>
#include <string_view>

namespace cronch {
enum class Lane : std::uint8_t { eLeft, eUp, eRight, eDown, eCOUNT_ };
constexpr util::EnumArray<Lane, std::string_view> lane_str_v = {"left", "up", "right", "down"};
constexpr Lane increment(Lane const in) { return static_cast<Lane>(static_cast<std::uint8_t>(in) + 1); }

constexpr glm::vec2 to_vec2(Lane const lane) {
	switch (lane) {
	default:
	case Lane::eLeft: return {-1.0f, 0.0f};
	case Lane::eUp: return {0.0f, 1.0f};
	case Lane::eRight: return {1.0f, 0.0f};
	case Lane::eDown: return {0.0f, -1.0f};
	}
}
} // namespace cronch
