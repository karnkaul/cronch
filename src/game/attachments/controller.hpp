#pragma once
#include <game/attachments/shared_prop.hpp>
#include <game/chomp_type.hpp>
#include <game/lane.hpp>
#include <game/layout.hpp>
#include <ktl/fixed_vector.hpp>
#include <vulkify/instance/key_event.hpp>

namespace cronch {
class Controller : public SharedProp {
  public:
	static constexpr std::size_t queue_size_v{4};

	enum class State : std::uint8_t { eIdle, eAdvance, eAttack, eRetreat, eCooldown };

	float speed{750.0f};
	float max_disp{0.20f * static_cast<float>(layout::extent.x)};
	tg::Time cooldown{0.20s};
	bool listen_keys{true};

	State state() const { return m_state; }
	Lane lane() const { return m_dir.lane; }
	glm::vec2 dir() const { return m_dir.vec; }

	void push(Lane lane);

  private:
	void tick(tg::DeltaTime dt) override;

	void push_dirs();
	void advance(tg::DeltaTime dt);
	void retreat(tg::DeltaTime dt);
	void cool(tg::DeltaTime dt);
	void pop_dir();
	void test_hit();

	void score(glm::vec2 position, ChompType type);
	void take_damage(glm::vec2 position);

	bool try_advance(tg::DeltaTime dt);
	bool retreat_finished() const;

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
	bool m_scored_hit{};
};
} // namespace cronch
