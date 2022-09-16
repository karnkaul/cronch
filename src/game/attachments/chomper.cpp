#include <engine/frame.hpp>
#include <game/attachments/chomper.hpp>
#include <game/attachments/player.hpp>
#include <game/layout.hpp>
#include <game/world.hpp>
#include <tardigrade/services.hpp>
#include <util/logger.hpp>
#include <algorithm>

namespace cronch {
Chomper::Instance Chomper::Factory::operator()() const {
	auto ret = Instance{};
	ret.sprite = AnimatedSprite{*tg::locate<vf::GfxDevice const*>()};
	ret.sprite.draw_invalid = true;
	return ret;
}

void Chomper::spawn(Lane const lane, vf::Radian const tumble, float const speed) {
	auto& vec = m_entries[lane];
	vec.push_back(m_pool.acquire());
	prepare(vec.back(), lane, tumble, speed);
}

bool Chomper::did_score(Lane const lane) {
	struct {
		Ptr<Instance> inst{};
		float dist_sq{};
	} closest{};
	auto& vec = m_entries[lane];
	for (auto& inst : vec) {
		auto const dist_sq = glm::dot(inst.sprite.transform().position, inst.sprite.transform().position);
		if (!closest.inst || dist_sq < closest.dist_sq) { closest = {&inst, dist_sq}; }
	}
	auto* world = static_cast<World*>(scene());
	if (closest.inst && world->player->prop->rect().intersects(closest.inst->sprite.bounds())) {
		release(vec, closest.inst);
		return true;
	}
	return false;
}

bool Chomper::did_hit() {
	auto const* player = static_cast<World*>(scene())->player;
	for (auto& vec : m_entries.array) {
		for (auto* inst : make_scratch(vec)) {
			if (player->prop->rect().intersects(inst->sprite.bounds())) {
				// TODO
				release(vec, inst);
				return true;
			}
		}
	}
	return false;
}

void Chomper::tick(tg::DeltaTime const dt) {
	for (auto& vec : m_entries.array) {
		for (auto& inst : vec) {
			inst.sprite.transform().orientation.rotate(vf::Radian{inst.tumble.value * dt.scaled.count()});
			inst.sprite.transform().position += inst.speed * dt.scaled.count() * -inst.dir;
		}
	}
}

void Chomper::render(tg::RenderTarget const& target) const {
	auto const& frame = static_cast<Frame const&>(target);
	for (auto const& vec : m_entries.array) {
		for (auto const& inst : vec) { frame.draw(inst.sprite); }
	}
}

void Chomper::prepare(Instance& out, Lane const lane, vf::Radian tumble, float const speed) {
	static constexpr auto he = 0.5f * glm::vec2{layout::extent};
	out.dir = vf::nvec2{to_vec2(lane)};
	out.speed = speed;
	out.tumble = tumble;
	out.sprite.transform().position = out.dir * he;
	out.sprite.set_size({50.0f, 50.0f});
}

void Chomper::release(std::vector<Instance>& out, Ptr<Instance> const target) {
	m_pool.release(std::move(*target));
	std::erase_if(out, [target](Instance const& c) { return &c == target; });
}

auto Chomper::make_scratch(std::span<Instance> const vec) -> std::span<Ptr<Instance>> {
	m_scratch.clear();
	m_scratch.reserve(vec.size());
	for (auto& inst : vec) { m_scratch.push_back(&inst); }
	return m_scratch;
}
} // namespace cronch
