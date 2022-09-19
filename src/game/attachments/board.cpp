#include <engine/frame.hpp>
#include <engine/resources.hpp>
#include <game/attachments/board.hpp>
#include <game/layout.hpp>
#include <game/theme.hpp>
#include <game/world.hpp>
#include <tardigrade/director.hpp>
#include <tardigrade/services.hpp>
#include <util/random.hpp>
#include <algorithm>

namespace cronch {
namespace {
template <typename T>
auto get_closest(T&& entries) {
	struct {
		Ptr<std::remove_reference_t<decltype(entries[0])>> entry{};
		float dist_sq{};
	} ret{};
	for (auto& entry : entries) {
		auto const dist_sq = glm::dot(entry.sprite.transform().position, entry.sprite.transform().position);
		if (!ret.entry || dist_sq < ret.dist_sq) { ret = {&entry, dist_sq}; }
	}
	return ret.entry;
}
} // namespace

void Board::Dilator::enable(float const scale, tg::Time const ttl) {
	tg::locate<tg::Director*>()->set_time_scale(scale);
	remain = ttl;
}

void Board::Dilator::tick(tg::DeltaTime const dt) {
	if (remain <= 0s) { return; }
	remain -= dt.real;
	if (remain <= 0s) { tg::locate<tg::Director*>()->set_time_scale(1.0f); }
}

Board::Entry Board::Factory::operator()() const {
	auto ret = Entry{};
	ret.sprite = vf::Sprite{*tg::locate<vf::GfxDevice const*>()};
	ret.sprite.draw_invalid = true;
	return ret;
}

void Board::spawn_food(Lane const lane, vf::Radian const tumble) {
	auto& vec = m_entries[lane];
	vec.push_back(m_pool.acquire());
	prepare(vec.back(), lane, {.speed = chomp_speed, .tumble = tumble, .type = ChompType::eFood});
}

void Board::spawn_dilator(Lane const lane, vf::Radian const tumble) {
	auto& vec = m_entries[lane];
	vec.push_back(m_pool.acquire());
	prepare(vec.back(), lane, {.speed = chomp_speed, .tumble = tumble, .type = ChompType::eDilator});
}

HitResult Board::attempt_hit(Lane const lane, vf::Rect const& rect) {
	auto& vec = m_entries[lane];
	auto* closest = get_closest(vec);
	if (!closest || !rect.intersects(closest->sprite.bounds())) { return {}; }
	auto const type = closest->chomp.type;
	auto const pos = closest->sprite.transform().position;
	release(vec, closest);
	return {pos, type, lane};
}

std::optional<vf::Rect> Board::raycast(Lane const lane) const {
	if (auto const* entry = get_closest(m_entries[lane])) { return entry->sprite.bounds(); }
	return {};
}

void Board::dilate_time(float const scale, tg::Time const duration) { m_dilator.enable(scale, duration); }

void Board::reset() {
	for (auto& entries : m_entries.array) { entries.clear(); }
	m_dilator.remain = 0s;
}

void Board::setup() {
	tg::RenderAttachment::setup();

	auto const* theme = tg::locate<Theme*>();
	m_sheet = tg::locate<Resources*>()->load<vf::Sprite::Sheet>(theme->chomps.assets.sheet);
	m_data = theme->chomps.data;
}

void Board::tick(tg::DeltaTime const dt) {
	for (auto& vec : m_entries.array) {
		for (auto& [sprite, chomp] : vec) {
			sprite.transform().orientation.rotate(vf::Radian{chomp.tumble.value * dt.scaled.count()});
			sprite.transform().position += chomp.speed * dt.scaled.count() * -chomp.dir;
		}
	}
	m_dilator.tick(dt);
}

void Board::render(tg::RenderTarget const& target) const {
	auto const& frame = static_cast<Frame const&>(target);
	for (auto const& vec : m_entries.array) {
		for (auto const& inst : vec) { frame.draw(inst.sprite); }
	}
}

void Board::prepare(Entry& out, Lane const lane, Chomp chomp) {
	static constexpr auto he = 0.5f * glm::vec2{layout::extent};
	out.chomp = std::move(chomp);
	out.chomp.dir = vf::nvec2{to_vec2(lane)};
	out.sprite.transform().position = out.chomp.dir * he;
	if (chomp.type == ChompType::eFood) {
		auto uv_index = std::uint32_t{m_data.uvs.first};
		if (m_data.uvs.last > m_data.uvs.first) { uv_index = util::random_range(m_data.uvs.first, m_data.uvs.last - 1); }
		out.sprite.set_sheet(m_sheet, vf::Sprite::UvIndex{uv_index});
		out.sprite.set_size(layout::food_size);
	} else {
		out.sprite.set_sheet(m_sheet, vf::Sprite::UvIndex{m_data.uvs.dilator});
		out.sprite.set_size(layout::dilator_size);
	}
}

void Board::release(std::vector<Entry>& out, Ptr<Entry> const target) {
	m_pool.release(std::move(*target));
	std::erase_if(out, [target](Entry const& c) { return &c == target; });
}

auto Board::make_scratch(std::span<Entry> const vec) -> std::span<Ptr<Entry>> {
	m_scratch.clear();
	m_scratch.reserve(vec.size());
	for (auto& inst : vec) { m_scratch.push_back(&inst); }
	return m_scratch;
}
} // namespace cronch
