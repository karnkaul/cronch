#include <game/attachments/autoplay.hpp>
#include <game/attachments/board.hpp>
#include <game/attachments/controller.hpp>
#include <game/attachments/player.hpp>
#include <game/world.hpp>
#include <glm/gtx/norm.hpp>

namespace cronch {
void AutoPlay::tick(tg::DeltaTime) {
	if (!enabled) { return; }
	auto* world = static_cast<World*>(scene());
	if (world->player->controller->state() != Controller::State::eIdle) { return; }
	struct {
		vf::Rect rect{};
		Lane lane{};
		float sqr_dist{};
		bool valid{};
	} target{};
	for (Lane l = Lane{}; l < Lane::eCOUNT_; l = increment(l)) {
		auto const closest = world->board->raycast(l);
		if (!closest) { continue; }
		auto const sqr_dist = glm::length2(world->player->prop->transform.position - closest->offset);
		if (!target.valid || sqr_dist < target.sqr_dist) { target = {*closest, l, sqr_dist, true}; }
	}
	if (!target.valid || !world->player->controller->can_chomp(target.rect)) { return; }

	world->player->controller->push(target.lane);
}
} // namespace cronch
