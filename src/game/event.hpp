#pragma once
#include <game/chomp_type.hpp>
#include <glm/vec2.hpp>

namespace cronch {
struct Event {
	struct Score;
	struct Damage;
	struct Reset;

	enum class Type { eScore, eDamage, eReset, eCOUNT_ };

	Type type;

	constexpr Event(Type type) : type{type} {}
};

struct Event::Score : Event {
	glm::vec2 position;
	ChompType chomp_type;

	constexpr Score(glm::vec2 const position, ChompType const chomp_type) : Event{Type::eScore}, position{position}, chomp_type{chomp_type} {}
};

struct Event::Damage : Event {
	glm::vec2 position;

	constexpr Damage(glm::vec2 const position) : Event{Type::eDamage}, position{position} {}
};

struct Event::Reset : Event {
	constexpr Reset() : Event{Type::eReset} {}
};
} // namespace cronch
