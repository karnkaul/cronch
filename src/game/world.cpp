#include <engine/resources.hpp>
#include <game/attachments/board.hpp>
#include <game/attachments/coordinator.hpp>
#include <game/attachments/dispatch.hpp>
#include <game/attachments/hud.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/vfx.hpp>
#include <game/theme.hpp>
#include <game/world.hpp>
#include <tardigrade/services.hpp>

namespace cronch {
void World::setup() {
	auto* entity = spawn();
	dispatch = entity->attach<Dispatch>();

	entity = spawn();
	player = entity->attach<Player>();

	entity = spawn();
	board = entity->attach<Board>();

	entity = spawn();
	hud = entity->attach<Hud>();

	entity = spawn();
	puff = entity->attach<Vfx>();

	entity = spawn();
	coordinator = entity->attach<Coordinator>();

	auto const* theme = tg::locate<Theme*>();
	puff->sheet = tg::locate<Resources*>()->load<vf::Sprite::Sheet>(theme->vfx.puff.sheet);
	puff->ttl = tg::Time{theme->vfx.puff.duration};
}
} // namespace cronch
