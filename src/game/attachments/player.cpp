#include <engine/frame.hpp>
#include <engine/resources.hpp>
#include <game/attachments/board.hpp>
#include <game/attachments/controller.hpp>
#include <game/attachments/hud.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/renderer.hpp>
#include <game/attachments/vfx.hpp>
#include <game/layout.hpp>
#include <game/theme.hpp>
#include <game/world.hpp>
#include <ktl/kformat.hpp>

namespace cronch {
bool Player::can_hit(Lane lane) const { return static_cast<World*>(scene())->board->can_hit(lane); }

HitResult Player::attempt_hit(Lane lane) { return static_cast<World*>(scene())->board->attempt_hit(lane, prop->rect()); }

std::optional<vf::Rect> Player::raycast(Lane const lane) const { return static_cast<World*>(scene())->board->raycast(lane); }

void Player::score(glm::vec2 position, ChompType const type) {
	auto* world = static_cast<World*>(scene());
	if (type == ChompType::eDilator) {
		m_dilators = std::clamp(m_dilators + 1, 0, layout::max_dilators_v);
	} else {
		world->hud->popup(ktl::kformat("{}x", m_score.add()), prop->transform.position);
	}
	world->puff->spawn(position);
}

bool Player::try_dilate_time() {
	auto* board = static_cast<World*>(scene())->board;
	if (board->dilator_enabled() || m_dilators == 0) { return false; }
	--m_dilators;
	board->dilate_time(dilation.scale, dilation.duration);
	return true;
}

void Player::take_damage(glm::vec2 position) {
	auto* world = static_cast<World*>(entity()->scene());
	reset_multiplier();
	world->puff->spawn(position, vf::red_v);
	// TODO: game over
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
