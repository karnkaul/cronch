#pragma once
#include <engine/animated_sprite.hpp>
#include <game/attachments/shared_prop.hpp>
#include <game/theme.hpp>
#include <util/cache.hpp>

namespace cronch {
class Controller;

template <std::derived_from<vf::Sprite> Type>
class SpriteRenderer;

class Player : public SharedProp {
  public:
	static constexpr int max_dilators_v{3};

	struct Dilation {
		tg::Time duration{3s};
		float scale{0.5f};
	};

	Dilation dilation{};
	Ptr<Controller> controller{};
	Ptr<SpriteRenderer<vf::Sprite>> sprite{};
	Ptr<vf::Sprite::Sheet> sheet{};

	void score_dilator();
	bool try_dilate_time();

	int dilator_count() const { return m_dilators; }

  private:
	void setup() override;
	void tick(tg::DeltaTime dt) override;

	Cache<std::uint32_t> m_uv{};
	Theme::Player::Data m_data{};
	int m_dilators{};
};
} // namespace cronch
