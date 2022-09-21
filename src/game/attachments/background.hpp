#pragma once
#include <tardigrade/render_attachment.hpp>
#include <vulkify/graphics/primitives/mesh.hpp>

namespace cronch {
class Background : public tg::RenderAttachment {
	void setup() override;
	void tick(tg::DeltaTime) override {}
	void render(tg::RenderTarget const& target) const override;

	vf::Mesh m_mesh{};
};
} // namespace cronch
