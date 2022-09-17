#pragma once
#include <engine/animated_sprite.hpp>
#include <game/attachments/shared_prop.hpp>
#include <util/cache.hpp>

namespace cronch {
class Controller;

template <std::derived_from<vf::Sprite> Type>
class SpriteRenderer;

class Player : public SharedProp {
  public:
	static constexpr int max_dilates_v{3};

	struct Dilation {
		tg::Time duration{3s};
		float scale{0.5f};
	};

	Dilation dilation{};
	Ptr<Controller> controller{};
	Ptr<SpriteRenderer<vf::Sprite>> sprite{};
	Ptr<vf::Sprite::Sheet> sheet{};

	void score_dilate();
	bool try_dilate_time();

  private:
	void setup() override;
	void tick(tg::DeltaTime dt) override;

	Cache<vf::Sprite::UvIndex> m_uv{};
	int m_dilates{};
};
} // namespace cronch
