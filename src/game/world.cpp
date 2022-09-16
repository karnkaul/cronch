#include <game/attachments/chomper.hpp>
#include <game/attachments/player.hpp>
#include <game/world.hpp>

namespace cronch {
void World::setup() {
	auto* entity = spawn();
	player = entity->attach<Player>();
	entity = spawn();
	chomper = entity->attach<Chomper>();
}
} // namespace cronch
