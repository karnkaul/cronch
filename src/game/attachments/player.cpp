#include <engine/frame.hpp>
#include <game/attachments/board.hpp>
#include <game/attachments/controller.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/renderer.hpp>
#include <game/world.hpp>

namespace cronch {
void Player::score_dilate() { m_dilates = std::clamp(max_dilates_v, 0, m_dilates + 1); }

bool Player::try_dilate_time() {
	if (m_dilates == 0) { return false; }
	--m_dilates;
	static_cast<World*>(scene())->board->dilate_time(dilation.scale, dilation.duration);
	return true;
}

void Player::setup() {
	SharedProp::setup();

	controller = entity()->attach<Controller>();
	sprite = entity()->find_or_attach<SpriteRenderer<vf::Sprite>>();

	sprite->get().draw_invalid = true;
	prop->bounds = {100.0f, 100.0f};
}

void Player::tick(tg::DeltaTime) {
	if (sheet) {
		auto const uv = controller->state() == Controller::State::eAttack ? vf::Sprite::UvIndex{1} : vf::Sprite::UvIndex{0};
		if (m_uv.changed(uv)) { sprite->get().set_sheet(sheet, uv); }
	}
	prop->transform.orientation = vf::nvec2{controller->dir()};
}
} // namespace cronch
