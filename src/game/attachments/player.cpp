#include <engine/frame.hpp>
#include <game/attachments/chomper.hpp>
#include <game/attachments/controller.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/renderer.hpp>
#include <game/world.hpp>

namespace cronch {
void Player::setup() {
	SharedProp::setup();

	controller = entity()->attach<Controller>();
	sprite = entity()->find_or_attach<SpriteRenderer<AnimatedSprite>>();

	sprite->get().draw_invalid = true;
	prop->bounds = {100.0f, 100.0f};
}

void Player::tick(tg::DeltaTime) {}
} // namespace cronch
