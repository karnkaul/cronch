#pragma once
#include <game/attachments/shared_prop.hpp>
#include <game/lane.hpp>
#include <ktl/fixed_vector.hpp>
#include <vulkify/instance/key_event.hpp>

namespace cronch {
using namespace std::chrono_literals;

class Controller : public SharedProp {
  public:
	static constexpr std::size_t queue_size_v{4};

	enum class State : std::uint8_t { eIdle, eAdvance, eAttack, eRetreat, eCooldown };

	float speed{1000.0f};
	float max_disp{100.0f};
	tg::Time cooldown{0.35s};
	bool listen_keys{true};

	State state() const { return m_state; }
	Lane lane() const { return m_lane; }

	void push(Lane lane);

  private:
	void tick(tg::DeltaTime dt) override;

	void push_keys();
	void advance(tg::DeltaTime dt);
	void attack(tg::DeltaTime dt);
	void retreat(tg::DeltaTime dt);
	void cool(tg::DeltaTime dt);
	void pop_dir();

	bool try_advance(tg::DeltaTime dt);
	bool retreat_finished() const;

	ktl::fixed_vector<Lane, queue_size_v> m_queue{};
	glm::vec2 m_dir{};
	tg::Time m_cooldown_remain{};
	State m_state{};
	bool m_scored_hit{};
	Lane m_lane{};
};
} // namespace cronch
