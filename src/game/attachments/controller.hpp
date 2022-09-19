#pragma once
#include <game/chomp_type.hpp>
#include <game/lane.hpp>
#include <game/layout.hpp>
#include <ktl/fixed_vector.hpp>
#include <tardigrade/tick_attachment.hpp>
#include <util/common.hpp>
#include <vulkify/core/rect.hpp>
#include <vulkify/instance/key_event.hpp>

namespace cronch {
using namespace std::chrono_literals;

class Player;
class Board;

class Controller : public tg::TickAttachment {
  public:
	enum Flag : std::uint32_t { eListenKeys = 1 << 0, eDisabled = 1 << 1 };

	static constexpr std::size_t queue_size_v{4};

	enum class State : std::uint8_t { eIdle, eAdvance, eAttack, eRetreat, eCooldown };

	float speed{750.0f};
	float max_disp{0.20f * static_cast<float>(layout::extent.x)};
	tg::Time cooldown{0.20s};
	std::uint32_t flags{eListenKeys};

	State state() const { return m_state; }
	Lane lane() const { return m_dir.lane; }
	glm::vec2 dir() const { return m_dir.vec; }

	bool can_chomp(vf::Rect const& target) const;
	void push(Lane lane);
	void reset();

  private:
	void setup() override {}
	void tick(tg::DeltaTime dt) override;

	void push_dirs();
	void advance(tg::DeltaTime dt);
	void retreat(tg::DeltaTime dt);
	void cool(tg::DeltaTime dt);

	void pop_dir();
	bool try_advance(tg::DeltaTime dt);
	bool retreat_finished() const;
	void test_hit();

	void score(glm::vec2 position, ChompType type);
	void damage(glm::vec2 position);
	bool refresh();

	struct Dir {
		Lane lane{Lane::eRight};
		glm::vec2 vec{1.0f, 0.0f};

		Dir() = default;
		Dir(Lane lane) noexcept;
	};

	ktl::fixed_vector<Lane, queue_size_v> m_queue{};
	Dir m_dir{};
	tg::Time m_cooldown_remain{};
	State m_state{};
	Ptr<Player> m_player{};
	Ptr<Board> m_board{};
	bool m_scored_hit{};
};
} // namespace cronch
