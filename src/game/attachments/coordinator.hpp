#pragma once
#include <game/event.hpp>
#include <tardigrade/tick_attachment.hpp>

namespace cronch {
class Coordinator : public tg::TickAttachment {
	void setup() override;
	void tick(tg::DeltaTime) override;

	void score(Event::Score const& score);
	void damage(Event::Damage const& damage);
	void reset();
};
} // namespace cronch
