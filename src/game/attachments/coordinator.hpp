#pragma once
#include <game/event.hpp>
#include <tardigrade/tick_attachment.hpp>

namespace cronch {
using namespace std::chrono_literals;

class Coordinator : public tg::TickAttachment {
	void setup() override;
	void tick(tg::DeltaTime) override;

	void score(Event::Score const& score);
	void damage(Event::Damage const& damage);
	void reset();

	static constexpr tg::Time restart_delay_v{2s};

	tg::Time m_dead_elapsed{};
};
} // namespace cronch
