#pragma once
#include <glm/vec2.hpp>

namespace cronch {
namespace layout {
inline constexpr glm::uvec2 extent{720U, 720U};
inline constexpr glm::vec2 centre{0.5f * glm::vec2{extent}};
inline constexpr glm::vec2 player_size{100.0f};
inline constexpr glm::vec2 food_size{75.0f};
inline constexpr glm::vec2 dilator_size{50.0f};
inline constexpr int max_dilators_v{3};
inline constexpr int max_health_v{3};

inline constexpr int title_height{80};
inline constexpr int subtitle_height{35};
inline constexpr char const* game_title_v{"cronch"};
} // namespace layout

namespace layer {
inline constexpr int background{-10};
inline constexpr int game{0};
inline constexpr int player{10};
inline constexpr int vfx{20};
inline constexpr int hud{30};
} // namespace layer
} // namespace cronch
