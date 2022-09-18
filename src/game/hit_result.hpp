#pragma once
#include <game/chomp_type.hpp>
#include <game/lane.hpp>
#include <glm/vec2.hpp>

namespace cronch {
struct HitResult {
	glm::vec2 position{};
	ChompType type{};
	Lane lane{};

	explicit constexpr operator bool() const { return type > ChompType::eNone; }
};
} // namespace cronch
