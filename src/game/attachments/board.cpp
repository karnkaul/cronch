#include <engine/frame.hpp>
#include <engine/resources.hpp>
#include <game/attachments/board.hpp>
#include <game/attachments/player.hpp>
#include <game/layout.hpp>
#include <game/theme.hpp>
#include <game/world.hpp>
#include <tardigrade/director.hpp>
#include <tardigrade/services.hpp>
#include <util/random.hpp>
#include <algorithm>

namespace cronch {
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
	ret.sprite = AnimatedSprite{*tg::locate<vf::GfxDevice const*>()};
	ret.sprite.draw_invalid = true;
	return ret;
}

void Board::spawn_food(Lane const lane, vf::Radian const tumble, float const speed) {
	auto& vec = m_entries[lane];
	vec.push_back(m_pool.acquire());
	prepare(vec.back(), lane, {.speed = speed, .tumble = tumble, .type = ChompType::eFood});
}

void Board::spawn_dilator(Lane const lane, vf::Radian const tumble, float const speed) {
	auto& vec = m_entries[lane];
	vec.push_back(m_pool.acquire());
	prepare(vec.back(), lane, {.speed = speed, .tumble = tumble, .type = ChompType::eDilator});
}

ChompType Board::try_score(Lane const lane) {
	struct {
		Ptr<Entry> entry{};
		float dist_sq{};
	} closest{};
	auto& vec = m_entries[lane];
	for (auto& entry : vec) {
		auto const dist_sq = glm::dot(entry.sprite.transform().position, entry.sprite.transform().position);
		if (!closest.entry || dist_sq < closest.dist_sq) { closest = {&entry, dist_sq}; }
	}
	auto* world = static_cast<World*>(scene());
	if (closest.entry && world->player->prop->rect().intersects(closest.entry->sprite.bounds())) {
		auto const ret = closest.entry->chomp.type;
		release(vec, closest.entry);
		return ret;
	}
	return ChompType::eNone;
}

ChompType Board::try_hit() {
	auto const* player = static_cast<World*>(scene())->player;
	for (auto& vec : m_entries.array) {
		for (auto* entry : make_scratch(vec)) {
			if (player->prop->rect().intersects(entry->sprite.bounds())) {
				// TODO
				auto const ret = entry->chomp.type;
				release(vec, entry);
				return ret;
			}
		}
	}
	return ChompType::eNone;
}

void Board::dilate_time(float const scale, tg::Time const duration) { m_dilator.enable(scale, duration); }

void Board::setup() {
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
		out.sprite.set_size(m_data.sizes.food);
	} else {
		out.sprite.set_sheet(m_sheet, vf::Sprite::UvIndex{m_data.uvs.dilator});
		out.sprite.set_size(m_data.sizes.dilator);
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
