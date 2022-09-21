#pragma once
#include <glm/vec2.hpp>
#include <string>

namespace cronch {
struct Theme {
	struct Player {
		struct Assets {
			std::string sheet{};
		};

		struct Data {
			struct {
				std::uint32_t idle{0};
				std::uint32_t attack{1};
				std::uint32_t heart{2};
			} uvs{};
		};

		Assets assets{};
		Data data{};
	};

	struct Chomps {
		struct Assets {
			std::string sheet{};
		};

		struct Data {
			struct {
				std::uint32_t first{};
				std::uint32_t last{};
				std::uint32_t dilator{};
			} uvs{};
		};

		Assets assets{};
		Data data{};
	};

	struct Hud {
		struct Assets {
			std::string font{};
		};

		struct Data {
			std::uint32_t score_height{60};
		};

		Assets assets{};
		Data data{};
	};

	struct Vfx {
		struct Data {
			std::string sheet{};
			float duration{0.25f};
		};

		Data puff{};
	};

	struct Background {
		struct Assets {
			std::string tile{};
		};

		Assets assets{};
	};

	Chomps chomps{};
	Player player{};
	Hud hud{};
	Vfx vfx{};
	Background background{};

	std::size_t load(char const* uri);
};
} // namespace cronch
