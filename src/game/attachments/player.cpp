#include <engine/frame.hpp>
#include <engine/resources.hpp>
#include <game/attachments/controller.hpp>
#include <game/attachments/dispatch.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/renderer.hpp>
#include <game/layout.hpp>
#include <game/theme.hpp>
#include <game/world.hpp>

namespace cronch {
std::int64_t Player::score_food() { return m_storage.score.add(); }

void Player::score_dilator() { m_storage.dilators = std::clamp(m_storage.dilators + 1, 0, layout::max_dilators_v); }

void Player::take_damage() {
	reset_multiplier();
	controller->flags |= Controller::eDisabled;
	m_storage.state = State::eDead;
}

bool Player::consume_dilator() {
	if (m_storage.dilators == 0) { return false; }
	--m_storage.dilators;
	return true;
}

void Player::reset() {
	m_storage = {};
	controller->reset();
}

void Player::setup() {
	prop = entity()->find_or_attach<Prop>();
	controller = entity()->attach<Controller>();
	sprite = entity()->attach<Renderer<vf::Sprite>>();

	auto const* theme = tg::locate<Theme*>();
	m_data = theme->player.data;
	sheet = tg::locate<Resources*>()->load<vf::Sprite::Sheet>(theme->player.assets.sheet);
	sprite->get().set_sheet(sheet, vf::Sprite::UvIndex{m_data.uvs.idle});

	sprite->get().draw_invalid = true;
	prop->bounds = layout::player_size;
}

void Player::tick(tg::DeltaTime) {
	auto const uv = controller->state() == Controller::State::eAttack ? m_data.uvs.attack : m_data.uvs.idle;
	if (m_uv.changed(uv)) { sprite->get().set_sheet(sheet, vf::Sprite::UvIndex{uv}); }
	prop->transform.orientation = vf::nvec2{controller->dir()};
}
} // namespace cronch
