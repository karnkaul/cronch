#include <game/attachments/board.hpp>
#include <game/attachments/controller.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/vfx.hpp>
#include <game/world.hpp>
#include <glm/gtx/norm.hpp>
#include <util/logger.hpp>
#include <vulkify/instance/keyboard.hpp>
#include <algorithm>

namespace cronch {
Controller::Dir::Dir(Lane lane) noexcept : lane{lane}, vec{to_vec2(lane)} {}

void Controller::push(Lane const lane) {
	if (m_queue.has_space()) { m_queue.push_back(lane); }
}

void Controller::tick(tg::DeltaTime dt) {
	auto* world = static_cast<World*>(entity()->scene());
	if (listen_keys) { push_dirs(); }
	switch (m_state) {
	case State::eIdle: pop_dir(); break;
	case State::eAdvance: advance(dt); break;
	case State::eAttack: try_advance(dt); break;
	case State::eRetreat: retreat(dt); break;
	case State::eCooldown: cool(dt); break;
	}
	if (vf::keyboard::pressed(vf::Key::eSpace) && world->player->try_dilate_time()) { logger::debug("[Controller] time dilation enabled"); }
	test_hit();
}

void Controller::pop_dir() {
	if (!m_queue.empty()) {
		if (m_queue.size() > 1) { std::rotate(m_queue.begin(), m_queue.begin() + 1, m_queue.end()); }
		m_dir = m_queue.back();
		m_queue.pop_back();
		m_state = State::eAdvance;
	}
}

void Controller::test_hit() {
	auto* world = static_cast<World*>(entity()->scene());
	for (Lane lane = Lane{}; lane < Lane::eCOUNT_; lane = increment(lane)) {
		auto const result = world->board->test_hit(lane, world->player->prop->rect());
		if (!result) { continue; }
		if ((m_state == State::eAttack || m_state == State::eAdvance) && m_dir.lane == result.lane) {
			score(result.position, result.type);
		} else if (result.type == ChompType::eFood) {
			take_damage(result.position);
		}
	}
}

void Controller::score(glm::vec2 const position, ChompType const type) {
	auto* world = static_cast<World*>(entity()->scene());
	m_scored_hit = true;
	m_state = State::eRetreat;
	if (type == ChompType::eDilator) {
		world->player->score_dilator();
	} else {
		world->player->score_chomp();
	}
	logger::debug("[Chomper] scored [{}] on lane [{}]", chomp_type_str_v[type], lane_str_v[m_dir.lane]);
	world->puff->spawn(position);
}

void Controller::take_damage(glm::vec2 const position) {
	auto* world = static_cast<World*>(entity()->scene());
	world->puff->spawn(position, vf::red_v);
	logger::debug("[Controller] took [{}] damage", chomp_type_str_v[ChompType::eFood]);
	// TODO: game over
}

void Controller::push_dirs() {
	using vf::keyboard::pressed;
	if (pressed(vf::Key::eA) || pressed(vf::Key::eLeft)) { push(Lane::eLeft); }
	if (pressed(vf::Key::eW) || pressed(vf::Key::eUp)) { push(Lane::eUp); }
	if (pressed(vf::Key::eD) || pressed(vf::Key::eRight)) { push(Lane::eRight); }
	if (pressed(vf::Key::eS) || pressed(vf::Key::eDown)) { push(Lane::eDown); }
}

void Controller::advance(tg::DeltaTime dt) {
	auto* world = static_cast<World*>(entity()->scene());
	if (world->board->has_chomp(m_dir.lane)) { m_state = State::eAttack; }
	try_advance(dt);
}

void Controller::retreat(tg::DeltaTime dt) {
	prop->transform.position -= speed * dt.real.count() * m_dir.vec;
	if (retreat_finished()) {
		if (!m_scored_hit) {
			static_cast<World*>(scene())->player->reset_multiplier();
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
	prop->transform.position += speed * dt.real.count() * m_dir.vec;
	return true;
}

bool Controller::retreat_finished() const {
	switch (m_dir.lane) {
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
