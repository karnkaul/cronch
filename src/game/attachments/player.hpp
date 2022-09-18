#pragma once
#include <engine/animated_sprite.hpp>
#include <game/attachments/shared_prop.hpp>
#include <game/score.hpp>
#include <game/theme.hpp>
#include <util/cache.hpp>

namespace cronch {
class Controller;

template <typename Type>
class Renderer;

class Player : public SharedProp {
  public:
	struct Dilation {
		tg::Time duration{3s};
		float scale{0.5f};
	};

	Dilation dilation{};
	Ptr<Controller> controller{};
	Ptr<Renderer<vf::Sprite>> sprite{};
	Ptr<vf::Sprite::Sheet> sheet{};

	void score_food();
	void reset_multiplier() { m_score.reset_multiplier(); }
	void score_dilator();
	bool try_dilate_time();

	Score const& score() const { return m_score; }
	int dilator_count() const { return m_dilators; }

  private:
	void setup() override;
	void tick(tg::DeltaTime dt) override;

	Cache<std::uint32_t> m_uv{};
	Theme::Player::Data m_data{};
	Score m_score{};
	int m_dilators{};
};
} // namespace cronch
