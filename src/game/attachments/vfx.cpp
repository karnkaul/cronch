#include <engine/frame.hpp>
#include <engine/resources.hpp>
#include <game/attachments/vfx.hpp>
#include <game/layout.hpp>
#include <game/theme.hpp>
#include <tardigrade/services.hpp>
#include <vulkify/ttf/ttf.hpp>

namespace cronch {
auto Vfx::Factory::operator()() const -> Entry {
	auto ret = Entry{};
	ret.sprite = AnimatedSprite{*tg::locate<vf::GfxDevice const*>()};
	return ret;
}

void Vfx::spawn(glm::vec2 position) {
	auto& entry = m_entries.active.emplace_back(m_entries.pool.acquire());
	entry.sprite.transform().position = position;
	if (sheet) { entry.sprite.set_sheet(*sheet, ttl); }
	entry.ttl = ttl;
}

void Vfx::setup() { layer = layer::vfx; }

void Vfx::tick(tg::DeltaTime dt) {
	for (auto& entry : m_entries.active) {
		entry.sprite.tick(dt.real);
		entry.ttl -= dt.real;
	}
	auto const func = [this](Entry& p) {
		if (p.ttl <= 0s) {
			m_entries.pool.release(std::move(p));
			return true;
		}
		return false;
	};
	std::erase_if(m_entries.active, func);
}

void Vfx::render(tg::RenderTarget const& target) const {
	auto const& frame = static_cast<Frame const&>(target);
	for (auto const& entry : m_entries.active) { frame.draw(entry.sprite); }
}
} // namespace cronch
