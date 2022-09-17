#include <engine/frame.hpp>
#include <engine/resources.hpp>
#include <game/attachments/board.hpp>
#include <game/attachments/controller.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/renderer.hpp>
#include <game/theme.hpp>
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

	auto const* theme = tg::locate<Theme*>();
	sheet = tg::locate<Resources*>()->load<vf::Sprite::Sheet>(theme->player.assets.sheet);
	sprite->get().set_sheet(sheet, vf::Sprite::UvIndex{theme->player.data.uvs.idle});
	m_data = theme->player.data;

	sprite->get().draw_invalid = true;
	prop->bounds = {m_data.size};
}

void Player::tick(tg::DeltaTime) {
	if (sheet) {
		auto const uv = controller->state() == Controller::State::eAttack ? m_data.uvs.attack : m_data.uvs.idle;
		if (m_uv.changed(uv)) { sprite->get().set_sheet(sheet, vf::Sprite::UvIndex{uv}); }
	}
	prop->transform.orientation = vf::nvec2{controller->dir()};
}
} // namespace cronch
