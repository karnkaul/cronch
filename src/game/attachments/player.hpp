#pragma once
#include <game/attachments/prop.hpp>
#include <game/hit_result.hpp>
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
	enum class State { eAlive, eDead };

	struct Dilation {
		tg::Time duration{3s};
		float scale{0.5f};
	};

	Dilation dilation{};
	Ptr<Controller> controller{};
	Ptr<Renderer<vf::Sprite>> sprite{};
	Ptr<vf::Sprite::Sheet const> sheet{};
	Ptr<Prop> prop{};

	std::int64_t score_food();
	void score_dilator();
	void take_damage();
	bool consume_dilator();
	void reset_multiplier() { m_storage.score.reset_multiplier(); }

	State state() const { return m_storage.state; }
	Score const& score() const { return m_storage.score; }
	int dilator_count() const { return m_storage.dilators; }

	void reset();

  private:
	void setup() override;
	void tick(tg::DeltaTime dt) override;

	Cache<std::uint32_t> m_uv{};
	Theme::Player::Data m_data{};
	struct {
		Score score{};
		int dilators{};
		State state{};
	} m_storage{};
};
} // namespace cronch
