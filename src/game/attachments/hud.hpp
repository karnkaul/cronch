#pragma once
#include <tardigrade/render_attachment.hpp>
#include <util/common.hpp>
#include <util/pool.hpp>
#include <vulkify/graphics/primitives/text.hpp>

namespace cronch {
using namespace std::chrono_literals;

class Hud : public tg::RenderAttachment {
  public:
  private:
	void setup() override;
	void tick(tg::DeltaTime dt) override;
	void render(tg::RenderTarget const& target) const override;
};
} // namespace cronch
