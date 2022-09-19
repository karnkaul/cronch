#pragma once
#include <game/event.hpp>
#include <tardigrade/attachment.hpp>

namespace cronch {
class Coordinator : public tg::Attachment {
	void setup() override;

	void score(Event::Score const& score);
	void damage(Event::Damage const& damage);
	void reset();
};
} // namespace cronch
