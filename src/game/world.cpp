#include <engine/resources.hpp>
#include <game/attachments/board.hpp>
#include <game/attachments/hud.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/vfx.hpp>
#include <game/theme.hpp>
#include <game/world.hpp>
#include <tardigrade/services.hpp>

namespace cronch {
void World::setup() {
	auto* entity = spawn();
	player = entity->attach<Player>();

	entity = spawn();
	board = entity->attach<Board>();

	entity = spawn();
	hud = entity->attach<Hud>();

	entity = spawn();
	poof = entity->attach<Vfx>();

	auto const* theme = tg::locate<Theme*>();
	poof->sheet = tg::locate<Resources*>()->load<vf::Sprite::Sheet>(theme->vfx.poof.sheet);
	poof->ttl = tg::Time{theme->vfx.poof.duration};
}
} // namespace cronch
