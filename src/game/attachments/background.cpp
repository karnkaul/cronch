#include <engine/frame.hpp>
#include <engine/resources.hpp>
#include <game/attachments/background.hpp>
#include <game/layout.hpp>
#include <game/theme.hpp>
#include <tardigrade/services.hpp>
#include <vulkify/graphics/texture.hpp>

namespace cronch {
void Background::setup() {
	m_mesh = vf::Mesh{*tg::locate<vf::GfxDevice const*>()};
	auto* resources = tg::locate<Resources*>();
	auto const* theme = tg::locate<Theme*>();
	auto const* texture = resources->load<vf::Texture>(theme->background.assets.tile);
	if (!texture) { return; }

	auto const layout = glm::vec2{layout::extent};
	auto const extent = glm::vec2{texture->extent()};
	auto const cols = std::floor(layout.x / extent.x);
	auto const width = layout.x / cols;
	auto const size = glm::vec2{width, width * extent.y / extent.x};
	auto const rows = std::ceil(layout.y / size.y);

	auto geometry = vf::Geometry{};
	auto const top_left = 0.5f * (glm::vec2{-layout.x, layout.y} + glm::vec2{size.x, -size.y});
	auto origin = top_left;
	for (int i = 0; i < static_cast<int>(rows); ++i) {
		for (int j = 0; j < static_cast<int>(cols); ++j) {
			geometry.add_quad(vf::QuadCreateInfo{.size = size, .origin = origin});
			origin.x += size.x;
		}
		origin.x = top_left.x;
		origin.y -= size.y;
	}

	m_mesh.buffer.write(std::move(geometry));
	m_mesh.texture = texture->handle();

	layer = layer::background;
}

void Background::render(tg::RenderTarget const& target) const { static_cast<Frame const&>(target).draw(m_mesh); }
} // namespace cronch
