#include <game/attachments/chomper.hpp>
#include <game/attachments/controller.hpp>
#include <game/attachments/player.hpp>
#include <game/world.hpp>
#include <glm/gtx/norm.hpp>
#include <util/logger.hpp>
#include <vulkify/instance/keyboard.hpp>
#include <algorithm>

namespace cronch {
void Controller::push(Lane const lane) {
	if (m_queue.has_space()) { m_queue.push_back(lane); }
}

void Controller::tick(tg::DeltaTime dt) {
	auto* world = static_cast<World*>(entity()->scene());
	if (listen_keys) { push_keys(); }
	switch (m_state) {
	case State::eIdle: pop_dir(); break;
	case State::eAdvance: advance(dt); break;
	case State::eAttack: attack(dt); break;
	case State::eRetreat: retreat(dt); break;
	case State::eCooldown: cool(dt); break;
	}
	if (world->chomper->did_hit()) {
		// TODO
		logger::debug("[Controller] chomper damage");
	}
}

void Controller::pop_dir() {
	if (!m_queue.empty()) {
		if (m_queue.size() > 1) { std::rotate(m_queue.begin(), m_queue.begin() + 1, m_queue.end()); }
		m_lane = m_queue.back();
		m_dir = to_vec2(m_lane);
		m_queue.pop_back();
		m_state = State::eAdvance;
	}
}

void Controller::push_keys() {
	using vf::keyboard::pressed;
	if (pressed(vf::Key::eA) || pressed(vf::Key::eLeft)) { push(Lane::eLeft); }
	if (pressed(vf::Key::eW) || pressed(vf::Key::eUp)) { push(Lane::eUp); }
	if (pressed(vf::Key::eD) || pressed(vf::Key::eRight)) { push(Lane::eRight); }
	if (pressed(vf::Key::eS) || pressed(vf::Key::eDown)) { push(Lane::eDown); }
}

void Controller::advance(tg::DeltaTime dt) {
	auto* world = static_cast<World*>(entity()->scene());
	if (world->chomper->has_chomper(m_lane)) { m_state = State::eAttack; }
	try_advance(dt);
}

void Controller::attack(tg::DeltaTime dt) {
	if (!try_advance(dt)) { return; }
	auto* world = static_cast<World*>(entity()->scene());
	if (world->chomper->did_score(m_lane)) {
		m_scored_hit = true;
		m_state = State::eRetreat;
		logger::debug("[Chomper] score on lane {}", lane_str_v[m_lane]);
	}
}

void Controller::retreat(tg::DeltaTime dt) {
	prop->transform.position -= speed * dt.real.count() * m_dir;
	if (retreat_finished()) {
		if (!m_scored_hit) {
			m_state = State::eCooldown;
			m_cooldown_remain = cooldown;
		} else {
			m_scored_hit = false;
			m_state = State::eIdle;
		}
		prop->transform.position = {};
	}
}

bool Controller::try_advance(tg::DeltaTime dt) {
	if (glm::length2(prop->transform.position) > max_disp * max_disp) {
		m_scored_hit = false;
		m_state = State::eRetreat;
		return false;
	}
	prop->transform.position += speed * dt.real.count() * m_dir;
	return true;
}

bool Controller::retreat_finished() const {
	switch (m_lane) {
	case Lane::eLeft: return prop->transform.position.x >= 0.0f;
	case Lane::eUp: return prop->transform.position.y <= 0.0f;
	case Lane::eRight: return prop->transform.position.x <= 0.0f;
	case Lane::eDown: return prop->transform.position.y >= 0.0f;
	default: return false;
	}
}

void Controller::cool(tg::DeltaTime dt) {
	m_cooldown_remain -= dt.real;
	if (m_cooldown_remain <= 0s) {
		m_state = State::eIdle;
		m_queue.clear();
	}
}
} // namespace cronch
