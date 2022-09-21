#pragma once
#include <game/lane.hpp>
#include <ktl/fixed_vector.hpp>
#include <tardigrade/tick_attachment.hpp>
#include <util/common.hpp>
#include <vulkify/core/radian.hpp>
#include <optional>

namespace cronch {
using namespace std::chrono_literals;

class Board;

template <typename T>
struct Range {
	T lo{};
	T hi{};
};

struct Wave {
	struct Advance;

	static constexpr std::array<Lane, 4> all_lanes_v = {Lane::eLeft, Lane::eUp, Lane::eRight, Lane::eDown};

	ktl::fixed_vector<Lane, 4> lanes{Lane::eLeft, Lane::eRight};
	tg::Time ttl{10s};
	float chomp_speed{100.0f};
	float dilator_chance{0.2f};
	int dilator_gate{5};
	tg::Time cooldown{3s};
	Range<tg::Time> spawn_rate{0.5s, 1s};
	Range<vf::Degree> tumble{{-180.0f}, {180.0f}};

	tg::Time elapsed{};
	int number{};
};

class WaveGen : public tg::TickAttachment {
  public:
	enum class State : std::uint8_t { eActive, eCooldown };

	void advance();
	void reset();

	bool no_dilators{};
	bool progression{};

  private:
	void setup() override;
	void tick(tg::DeltaTime dt) override;

	tg::Time make_till_next() const;
	void spawn();

	Wave m_active{};
	std::optional<Lane> m_prev_lane{};
	Ptr<Board> m_board{};
	State m_state{State::eActive};
	tg::Time m_till_next{};
	tg::Time m_prev_till_next{};
	int m_dilator_gate{};
};
} // namespace cronch
