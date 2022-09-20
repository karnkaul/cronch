#pragma once
#include <game/attachments/prop.hpp>
#include <game/hit_result.hpp>
#include <game/layout.hpp>
#include <game/score.hpp>
#include <game/theme.hpp>
#include <tardigrade/tick_attachment.hpp>
#include <util/cache.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>

namespace cronch {
using namespace std::chrono_literals;

class Controller;

template <typename Type>
class Renderer;

class Player : public tg::TickAttachment {
  public:
	struct {
		tg::Time duration{3s};
		float scale{0.5f};
	} dilation{};
	struct {
		tg::Time ttl{2s};
		tg::Time pulse{0.05s};
	} immune{};

	Ptr<Controller> controller{};
	Ptr<Renderer<vf::Sprite>> sprite{};
	Ptr<vf::Sprite::Sheet const> sheet{};
	Ptr<Prop> prop{};

	std::int64_t score_food();
	void score_dilator();
	bool take_damage();
	bool consume_dilator();
	void reset_multiplier() { m_storage.score.reset_multiplier(); }

	int health() const { return m_storage.health; }
	Score const& score() const { return m_storage.score; }
	int dilator_count() const { return m_storage.dilators; }

	void reset();

  private:
	struct Immune {
		struct {
			tg::Time main{};
			tg::Time pulse{};
		} ttl{};

		struct {
			tg::Time elapsed{};
			bool on{};
		} pulse{};

		void enable(tg::Time main, tg::Time pulse);
		bool tick(tg::Time dt);
	};

	void setup() override;
	void tick(tg::DeltaTime dt) override;

	Cache<std::uint32_t> m_uv{};
	Theme::Player::Data m_data{};
	struct {
		Immune immune{};
		Score score{};
		int dilators{};
		int health{layout::max_health_v};
	} m_storage{};
};
} // namespace cronch
