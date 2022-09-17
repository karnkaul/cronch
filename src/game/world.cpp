#include <game/attachments/board.hpp>
#include <game/attachments/player.hpp>
#include <game/world.hpp>

namespace cronch {
void World::setup() {
	auto* entity = spawn();
	player = entity->attach<Player>();

	entity = spawn();
	board = entity->attach<Board>();
}
} // namespace cronch
