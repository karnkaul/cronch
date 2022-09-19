#include <game/attachments/board.hpp>
#include <game/attachments/controller.hpp>
#include <game/attachments/coordinator.hpp>
#include <game/attachments/dispatch.hpp>
#include <game/attachments/hud.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/vfx.hpp>
#include <game/world.hpp>
#include <ktl/kformat.hpp>

namespace cronch {
void Coordinator::setup() {
	auto* dispatch = static_cast<World*>(scene())->dispatch;
	dispatch->attach(entity()->id(), Event::Type::eScore, [this](Event const& e) { score(static_cast<Event::Score const&>(e)); });
	dispatch->attach(entity()->id(), Event::Type::eDamage, [this](Event const& e) { damage(static_cast<Event::Damage const&>(e)); });
	dispatch->attach(entity()->id(), Event::Type::eReset, [this](Event const&) { reset(); });
}

void Coordinator::score(Event::Score const& score) {
	auto* world = static_cast<World*>(scene());
	switch (score.chomp_type) {
	case ChompType::eFood: {
		auto text = ktl::kformat("{}x", world->player->score_food());
		world->hud->popup(std::move(text), world->player->prop->transform.position);
		break;
	}
	case ChompType::eDilator: {
		world->player->score_dilator();
		break;
	}
	default: break;
	}
	world->puff->spawn(score.position);
}

void Coordinator::damage(Event::Damage const& damage) {
	auto* world = static_cast<World*>(scene());
	world->player->take_damage();
	world->puff->spawn(damage.position, vf::red_v);
	world->board->reset();
}

void Coordinator::reset() {
	auto* world = static_cast<World*>(scene());
	world->player->reset();
	world->board->reset();
}
} // namespace cronch
