#pragma once
#include <engine/animated_sprite.hpp>
#include <game/attachments/shared_prop.hpp>

namespace cronch {
class Controller;

template <std::derived_from<vf::Sprite> Type>
class SpriteRenderer;

class Player : public SharedProp {
  public:
	Ptr<Controller> controller{};
	Ptr<SpriteRenderer<AnimatedSprite>> sprite{};

  private:
	void setup() override;
	void tick(tg::DeltaTime dt) override;
};
} // namespace cronch
