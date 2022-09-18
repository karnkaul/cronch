#include <engine/frame.hpp>
#include <engine/resources.hpp>
#include <game/attachments/hud.hpp>
#include <game/attachments/player.hpp>
#include <game/layout.hpp>
#include <game/theme.hpp>
#include <game/world.hpp>
#include <tardigrade/services.hpp>
#include <vulkify/ttf/ttf.hpp>

namespace cronch {
namespace {
std::string format_score(std::int64_t value) {
	auto str = std::to_string(value);
	auto ret = std::string{};
	int digits{};
	for (auto it = str.rbegin(); it != str.rend(); ++it) {
		ret += *it;
		if (++digits == 3 && it + 1 != str.rend()) {
			digits = 0;
			ret += ',';
		}
	}
	std::reverse(ret.begin(), ret.end());
	return ret;
}
} // namespace

auto Hud::Factory::operator()() const -> Popup {
	auto ret = Popup{};
	ret.text = vf::Text{*tg::locate<vf::GfxDevice const*>()};
	ret.text.set_ttf(ttf);
	return ret;
}

void Hud::popup(std::string text, glm::vec2 position, vf::Rgba const tint) {
	auto& popup = m_popups.active.emplace_back(m_popups.pool.acquire());
	popup.text.set_string(std::move(text));
	popup.text.transform().position = position;
	popup.text.tint() = tint;
	popup.text.set_height(popup_height);
	popup.ttl = popup_ttl;
}

void Hud::setup() {
	auto const* theme = tg::locate<Theme*>();
	auto* resources = tg::locate<Resources*>();
	auto* ttf = resources->load<vf::Ttf>(theme->hud.assets.font);
	auto const* sheet = resources->load<vf::Sprite::Sheet>(theme->chomps.assets.sheet);
	m_popups.pool.get_factory().ttf = ttf;

	auto const& device = *tg::locate<vf::GfxDevice const*>();
	m_score.text = vf::Text{device};
	m_score.text.set_height(vf::Text::Height{theme->hud.data.score_height});
	auto const he = 0.5f * glm::vec2{layout::extent};
	m_score.text.transform().position = glm::vec2{-he.x + 50.0f, he.y - 50.0f};
	m_score.text.set_align({.horz = vf::Text::Horz::eLeft, .vert = vf::Text::Vert::eMid});
	if (ttf) { m_score.text.set_ttf(ttf); }
	m_score.text.set_string("0");

	auto pos = he - glm::vec2{50.0f};
	auto const size = glm::vec2{static_cast<float>(theme->hud.data.score_height)};
	for (auto& dilator : m_dilators.sprites) {
		dilator = vf::Sprite{device};
		dilator.draw_invalid = true;
		dilator.set_sheet(sheet, vf::Sprite::UvIndex{theme->chomps.data.uvs.dilator});
		dilator.set_size(size);
		dilator.transform().position = pos;
		pos.x -= 2.0f * size.x;
	}

	layer = layer::hud;
}

void Hud::tick(tg::DeltaTime dt) {
	for (auto& popup : m_popups.active) {
		popup.ttl -= dt.real;
		popup.text.tint().channels[3] = static_cast<vf::Rgba::Channel>((popup.ttl / popup_ttl) * 0xff);
		popup.text.transform().position.y += dt.real.count() * popup_y_speed;
	}
	auto const func = [this](Popup& p) {
		if (p.ttl <= 0s) {
			m_popups.pool.release(std::move(p));
			return true;
		}
		return false;
	};
	std::erase_if(m_popups.active, func);
	auto const* world = static_cast<World const*>(scene());

	update_score(world->player->score().value);
	m_dilators.count = world->player->dilator_count();
}

void Hud::render(tg::RenderTarget const& target) const {
	auto const& frame = static_cast<Frame const&>(target);
	frame.draw(m_score.text);
	for (auto const& popup : m_popups.active) { frame.draw(popup.text); }
	for (int i = 0; i < m_dilators.count; ++i) { frame.draw(m_dilators.sprites[static_cast<std::size_t>(i)]); }
}

void Hud::update_score(std::int64_t current) {
	if (current != m_score.previous) {
		m_score.previous = current;
		m_score.text.set_string(format_score(current));
	}
}
} // namespace cronch
