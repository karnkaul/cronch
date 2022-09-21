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

void Hud::spawn(Toast toast) {
	auto& popup = m_popups.active.emplace_back(m_popups.pool.acquire());
	popup.text.set_string(std::move(toast.text));
	popup.text.transform().position = toast.position;
	popup.text.tint() = toast.tint;
	popup.text.set_height(popup_height);
	popup.elapsed = {};
	popup.ttl = toast.ttl;
	popup.tick = toast.tick;
}

void Hud::setup() {
	tg::RenderAttachment::setup();

	auto const* theme = tg::locate<Theme*>();
	auto* resources = tg::locate<Resources*>();
	auto* ttf = resources->load<vf::Ttf>(theme->hud.assets.font);
	m_popups.pool.get_factory().ttf = ttf;

	auto const& device = *tg::locate<vf::GfxDevice const*>();
	m_score.text = vf::Text{device};
	m_score.text.set_height(vf::Text::Height{theme->hud.data.score_height});
	auto const extent = glm::vec2{layout::extent};
	auto const he = 0.5f * extent;
	m_score.text.transform().position = glm::vec2{-he.x + 50.0f, he.y - 50.0f};
	m_score.text.set_align({.horz = vf::Text::Horz::eLeft, .vert = vf::Text::Vert::eMid});
	if (ttf) { m_score.text.set_ttf(ttf); }
	m_score.text.set_string("0");

	auto pos = he - glm::vec2{50.0f};
	auto const* sheet = resources->load<vf::Sprite::Sheet>(theme->player.assets.sheet);
	auto size = glm::vec2{static_cast<float>(theme->hud.data.score_height)};
	for (auto& heart : m_hearts.sprites) {
		heart = vf::Sprite{device};
		heart.draw_invalid = true;
		heart.set_sheet(sheet, vf::Sprite::UvIndex{theme->player.data.uvs.heart});
		heart.set_size(size);
		heart.transform().position = pos;
		pos.x -= 2.0f * size.x;
	}

	sheet = resources->load<vf::Sprite::Sheet>(theme->chomps.assets.sheet);
	size *= 0.75f;
	pos = he - glm::vec2{50.0f};
	pos.y -= 50.0f;
	for (auto& dilator : m_dilators.sprites) {
		dilator = vf::Sprite{device};
		dilator.draw_invalid = true;
		dilator.set_sheet(sheet, vf::Sprite::UvIndex{theme->chomps.data.uvs.dilator});
		dilator.set_size(size);
		dilator.transform().position = pos;
		pos.x -= 2.0f * size.x;
	}

	setup_letterbox(extent, 0.3f * extent.y);

	layer = layer::hud;
}

void Hud::tick(tg::DeltaTime dt) {
	for (auto& popup : m_popups.active) {
		popup.elapsed += dt.real;
		if (popup.tick) {
			auto const t = popup.elapsed / popup.ttl;
			popup.tick(popup.text, dt, t);
		}
	}
	auto const func = [this](Popup& p) {
		if (p.elapsed > p.ttl) {
			m_popups.pool.release(std::move(p));
			return true;
		}
		return false;
	};
	std::erase_if(m_popups.active, func);
	auto const* world = static_cast<World const*>(scene());

	update_score(world->player->score().value);
	m_dilators.count = world->player->dilator_count();
	m_hearts.count = world->player->health();

	update_letterbox(dt.real);
}

void Hud::render(tg::RenderTarget const& target) const {
	auto const& frame = static_cast<Frame const&>(target);
	frame.draw(m_score.text);
	for (auto const& popup : m_popups.active) { frame.draw(popup.text); }
	for (int i = 0; i < m_dilators.count; ++i) { frame.draw(m_dilators.sprites[static_cast<std::size_t>(i)]); }
	for (int i = 0; i < m_hearts.count; ++i) { frame.draw(m_hearts.sprites[static_cast<std::size_t>(i)]); }
	frame.draw(m_letterbox.top);
	frame.draw(m_letterbox.bottom);
}

void Hud::setup_letterbox(glm::vec2 const area, float slit) {
	m_letterbox.top = vf::Mesh{*tg::locate<vf::GfxDevice const*>()};
	m_letterbox.bottom = vf::Mesh{*tg::locate<vf::GfxDevice const*>()};
	auto const size = glm::vec2{area.x, 0.5f * (area.y - slit)} + 3.0f;
	m_letterbox.top.buffer.write(vf::Geometry::make_quad(vf::QuadCreateInfo{.size = size}));
	m_letterbox.bottom.buffer.write(vf::Geometry::make_quad(vf::QuadCreateInfo{.size = size}));
	m_letterbox.top.instance().tint = m_letterbox.bottom.instance().tint = vf::black_v;
	m_letterbox.top.instance().transform.position.y = 0.5f * (slit + size.y);
	m_letterbox.bottom.instance().transform.position.y = 0.5f * (-slit - size.y);
	m_letterbox.y_enabled = m_letterbox.top.instance().transform.position.y;
	m_letterbox.y_disabled = m_letterbox.y_enabled + size.y + 2.0f;
}

void Hud::update_score(std::int64_t current) {
	if (current != m_score.previous) {
		m_score.previous = current;
		m_score.text.set_string(format_score(current));
	}
}

void Hud::update_letterbox(tg::Time dt) {
	if (m_letterbox.enabled) {
		if (m_letterbox.top.instance().transform.position.y > m_letterbox.y_enabled) {
			m_letterbox.top.instance().transform.position.y -= letterbox_speed * dt.count();
		}
		if (m_letterbox.top.instance().transform.position.y < m_letterbox.y_enabled) {
			m_letterbox.top.instance().transform.position.y = m_letterbox.y_enabled;
		}
	} else {
		if (m_letterbox.top.instance().transform.position.y < m_letterbox.y_disabled) {
			m_letterbox.top.instance().transform.position.y += letterbox_speed * dt.count();
		}
		if (m_letterbox.top.instance().transform.position.y > m_letterbox.y_disabled) {
			m_letterbox.top.instance().transform.position.y = m_letterbox.y_disabled;
		}
	}
	m_letterbox.bottom.instance().transform.position.y = -m_letterbox.top.instance().transform.position.y;
}
} // namespace cronch
