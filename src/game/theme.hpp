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
			} uvs{};
			glm::vec2 size{100.0f};
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
			struct {
				glm::vec2 food{75.0f};
				glm::vec2 dilator{50.0f};
			} sizes{};
		};

		Assets assets{};
		Data data{};
	};

	Chomps chomps{};
	Player player{};

	std::size_t load(char const* uri);
};
} // namespace cronch
