#include <engine/frame.hpp>
#include <engine/resources.hpp>
#include <game/attachments/board.hpp>
#include <game/attachments/controller.hpp>
#include <game/attachments/hud.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/renderer.hpp>
#include <game/layout.hpp>
#include <game/theme.hpp>
#include <game/world.hpp>
#include <ktl/kformat.hpp>

namespace cronch {
void Player::score_food() {
	auto* world = static_cast<World*>(scene());
	world->hud->popup(ktl::kformat("{}x", m_score.add()), prop->transform.position);
}

void Player::score_dilator() { m_dilators = std::clamp(m_dilators + 1, 0, layout::max_dilators_v); }

bool Player::try_dilate_time() {
	auto* board = static_cast<World*>(scene())->board;
	if (board->dilator_enabled() || m_dilators == 0) { return false; }
	--m_dilators;
	board->dilate_time(dilation.scale, dilation.duration);
	return true;
}

void Player::setup() {
	SharedProp::setup();

	controller = entity()->attach<Controller>();
	sprite = entity()->find_or_attach<Renderer<vf::Sprite>>();

	auto const* theme = tg::locate<Theme*>();
	sheet = tg::locate<Resources*>()->load<vf::Sprite::Sheet>(theme->player.assets.sheet);
	sprite->get().set_sheet(sheet, vf::Sprite::UvIndex{theme->player.data.uvs.idle});
	m_data = theme->player.data;

	sprite->get().draw_invalid = true;
	prop->bounds = layout::player_size;
}

void Player::tick(tg::DeltaTime) {
	if (sheet) {
		auto const uv = controller->state() == Controller::State::eAttack ? m_data.uvs.attack : m_data.uvs.idle;
		if (m_uv.changed(uv)) { sprite->get().set_sheet(sheet, vf::Sprite::UvIndex{uv}); }
	}
	prop->transform.orientation = vf::nvec2{controller->dir()};
}
} // namespace cronch
