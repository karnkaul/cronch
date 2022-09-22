#pragma once
#include <game/chomp_type.hpp>
#include <glm/vec2.hpp>

namespace cronch::event {
struct Score {
	glm::vec2 position{};
	ChompType chomp_type{};
};

struct Damage {
	glm::vec2 position{};
};

struct Reset {};
} // namespace cronch::event
