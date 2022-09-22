#include <game/attachments/board.hpp>
#include <game/attachments/wave.hpp>
#include <game/world.hpp>
#include <tardigrade/services.hpp>
#include <util/logger.hpp>
#include <util/random.hpp>

namespace cronch {
namespace {
ktl::fixed_vector<Lane, 4> make_lanes(int const wave_number) {
	auto ret = ktl::fixed_vector<Lane, 4>{Lane::eLeft, Lane::eRight};
	if (wave_number >= 3) {
		ret.push_back(Lane::eUp);
		ret.push_back(Lane::eDown);
	}
	return ret;
}

Wave advance_wave(Wave const& in, bool progression) {
	if (in.number == 0) {
		auto ret = in;
		ret.number = 1;
		return ret;
	}
	if (!progression) { return in; }
	auto const number = in.number + 1;
	return Wave{
		.lanes = make_lanes(number),
		.ttl = std::min(in.ttl + 5.0s, 30.0s),
		.chomp_speed = std::min(in.chomp_speed + 25.0f, 300.0f),
		.dilator_chance = std::max(in.dilator_chance - 0.03f, 0.05f),
		.dilator_gate = std::min(in.dilator_gate + 2, 20),
		.cooldown = 3s,
		.spawn_rate =
			{
				.lo = std::max(in.spawn_rate.lo - 0.1s, 0.2s),
				.hi = std::max(in.spawn_rate.hi - 0.2s, 0.3s),
			},
		.number = number,
	};
}
} // namespace

std::string to_string(Wave const& wave) {
	return ktl::kformat("[Wave {}] ttl: {:.1f}s | speed: {:.1f} | dilator- chance: {:.2f}, gate: {} | spawn_rate: {:.2f}s-{:.2f}s", wave.number,
						wave.ttl.count(), wave.chomp_speed, wave.dilator_chance, wave.dilator_gate, wave.spawn_rate.lo.count(), wave.spawn_rate.hi.count());
}

void WaveGen::advance() { m_active.elapsed = m_state == State::eActive ? m_active.ttl : m_active.cooldown; }

void WaveGen::reset() {
	m_active = {};
	m_state = State::eCooldown;
	m_dilator_gate = 0;
	no_dilators = false;
	progression = true;
}

void WaveGen::setup() { reset(); }

void WaveGen::tick(tg::DeltaTime dt) {
	m_board = static_cast<World*>(scene())->board;
	auto const ttl = m_state == State::eActive ? m_active.ttl : m_active.cooldown;
	if (m_active.elapsed >= ttl) {
		m_active.elapsed = {};
		if (m_state == State::eActive && progression) {
			m_state = State::eCooldown;
			logger::debug("[Wave] cooldown for [{:.1f}s]", m_active.cooldown.count());
		} else {
			m_state = State::eActive;
			m_active = advance_wave(m_active, progression);
			m_till_next = make_till_next();
			m_board->chomp_speed = m_active.chomp_speed;
			logger::debug("{}", m_active);
		}
	}

	if (m_state == State::eActive) {
		m_till_next -= dt.real;
		if (m_till_next <= 0s) {
			spawn();
			m_till_next = make_till_next();
			m_prev_till_next = m_till_next;
		}
	}

	m_active.elapsed += dt.real;
}

void WaveGen::spawn() {
	auto* world = static_cast<World*>(scene());
	auto const toss = util::random_range(0.0f, 1.0f);
	auto temp = decltype(m_active.lanes){};
	for (auto const lane : m_active.lanes) {
		if (m_prev_lane && *m_prev_lane == lane) { continue; }
		temp.push_back(lane);
	}
	auto const lane_index = util::random_range<std::size_t>(0, temp.size() - 1);
	assert(lane_index < temp.size());
	auto const lane = temp[lane_index];
	auto const tumble = vf::Degree{util::random_range(m_active.tumble.lo.value, m_active.tumble.hi.value)};
	m_prev_lane = lane;
	if (!no_dilators && toss < m_active.dilator_chance && m_dilator_gate <= 0) {
		world->board->spawn_dilator(lane, tumble);
		m_dilator_gate = m_active.dilator_gate;
	} else {
		world->board->spawn_food(lane, tumble);
		--m_dilator_gate;
	}
	logger::debug("[Wave] ttl: [{:.2f}s] spawn: after [{:.2f}s], lane: [{}]", (m_active.ttl - m_active.elapsed).count(), m_prev_till_next.count(),
				  lane_str_v[lane]);
}

tg::Time WaveGen::make_till_next() const { return tg::Time{util::random_range(m_active.spawn_rate.lo.count(), m_active.spawn_rate.hi.count())}; }
} // namespace cronch
