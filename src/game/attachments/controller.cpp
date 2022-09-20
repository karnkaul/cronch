#include <game/attachments/board.hpp>
#include <game/attachments/controller.hpp>
#include <game/attachments/dispatch.hpp>
#include <game/attachments/player.hpp>
#include <game/world.hpp>
#include <glm/gtx/norm.hpp>
#include <util/logger.hpp>
#include <util/random.hpp>
#include <vulkify/instance/keyboard.hpp>
#include <algorithm>

namespace cronch {
namespace {
glm::vec2 randomize(glm::vec2 const range) { return {util::random_range(-range.x, range.x), util::random_range(-range.y, range.y)}; }
} // namespace

Controller::Dir::Dir(Lane lane) noexcept : lane{lane}, vec{to_vec2(lane)} {}

bool Controller::can_chomp(vf::Rect const& target) const {
	auto* prop = m_player->prop;
	auto to_target = target.offset - prop->transform.position;
	if (glm::length2(to_target) < max_disp * max_disp) { return true; }
	to_target = glm::normalize(to_target);
	auto const rect = vf::Rect{{prop->bounds, max_disp * to_target}};
	return rect.intersects(target);
}

void Controller::push(Lane const lane) {
	if (m_queue.has_space()) { m_queue.push_back(lane); }
}

void Controller::reset() {
	flags &= ~eDisabled;
	m_queue.clear();
	m_position = {};
	m_cooldown_remain = {};
	if (m_state != State::eIdle) {
		m_scored_hit = true;
		m_state = State::eRetreat;
	}
}

void Controller::tick(tg::DeltaTime dt) {
	if ((flags & eDisabled) || !refresh()) { return; }

	m_player->prop->transform.position = m_position;
	if (flags & eListenKeys) { push_dirs(); }

	switch (m_state) {
	case State::eIdle: pop_dir(); break;
	case State::eAdvance: advance(dt); break;
	case State::eAttack: try_advance(dt); break;
	case State::eRetreat: retreat(dt); break;
	case State::eCooldown: cool(dt); break;
	}

	if (m_board->dilator_enabled()) {
		m_player->prop->transform.position += randomize({vibrate, vibrate});
	} else {
		if (vf::keyboard::pressed(vf::Key::eSpace) && m_player->consume_dilator()) {
			m_board->dilate_time(m_player->dilation.scale, m_player->dilation.duration);
		}
	}
	test_hit();
}

void Controller::push_dirs() {
	using vf::keyboard::pressed;
	if (pressed(vf::Key::eA) || pressed(vf::Key::eLeft)) { push(Lane::eLeft); }
	if (pressed(vf::Key::eW) || pressed(vf::Key::eUp)) { push(Lane::eUp); }
	if (pressed(vf::Key::eD) || pressed(vf::Key::eRight)) { push(Lane::eRight); }
	if (pressed(vf::Key::eS) || pressed(vf::Key::eDown)) { push(Lane::eDown); }
}

void Controller::advance(tg::DeltaTime dt) {
	if (m_board->can_hit(m_dir.lane)) { m_state = State::eAttack; }
	try_advance(dt);
}

void Controller::retreat(tg::DeltaTime dt) {
	m_position -= speed * dt.real.count() * m_dir.vec;
	if (retreat_finished()) {
		if (!m_scored_hit) {
			m_player->reset_multiplier();
			m_state = State::eCooldown;
			m_cooldown_remain = cooldown;
		} else {
			m_scored_hit = false;
			m_state = State::eIdle;
		}
		m_position = {};
	}
}

void Controller::cool(tg::DeltaTime dt) {
	m_cooldown_remain -= dt.real;
	if (m_cooldown_remain <= 0s) {
		m_state = State::eIdle;
		m_queue.clear();
	}
}

void Controller::pop_dir() {
	if (!m_queue.empty()) {
		if (m_queue.size() > 1) { std::rotate(m_queue.begin(), m_queue.begin() + 1, m_queue.end()); }
		m_dir = m_queue.back();
		m_queue.pop_back();
		m_state = State::eAdvance;
	}
}

bool Controller::try_advance(tg::DeltaTime dt) {
	if (glm::length2(m_position) > max_disp * max_disp) {
		m_scored_hit = false;
		m_state = State::eRetreat;
		return false;
	}
	m_position += speed * dt.real.count() * m_dir.vec;
	return true;
}

bool Controller::retreat_finished() const {
	auto* prop = m_player->prop;
	switch (m_dir.lane) {
	case Lane::eLeft: return prop->transform.position.x >= 0.0f;
	case Lane::eUp: return prop->transform.position.y <= 0.0f;
	case Lane::eRight: return prop->transform.position.x <= 0.0f;
	case Lane::eDown: return prop->transform.position.y >= 0.0f;
	default: return false;
	}
}

void Controller::score(glm::vec2 const position, ChompType const type) {
	m_scored_hit = true;
	m_state = State::eRetreat;
	static_cast<World*>(scene())->dispatch->dispatch(Event::Score{position, type});
}

void Controller::damage(glm::vec2 const position) { static_cast<World*>(scene())->dispatch->dispatch(Event::Damage{position}); }

void Controller::test_hit() {
	for (Lane lane = Lane{}; lane < Lane::eCOUNT_; lane = increment(lane)) {
		auto const result = m_board->attempt_hit(lane, m_player->prop->rect());
		if (!result) { continue; }
		if (m_state != State::eIdle && m_dir.lane == result.lane) {
			score(result.position, result.type);
		} else if (result.type == ChompType::eFood) {
			damage(result.position);
		}
	}
}

bool Controller::refresh() {
	m_player = entity()->find<Player>();
	if (!m_player) {
		logger::warn("[Controller] Player not attached");
		return false;
	}
	m_board = static_cast<World*>(scene())->board;
	if (!m_board) {
		logger::warn("[Controller] Board not attached");
		return false;
	}
	return true;
}
} // namespace cronch
