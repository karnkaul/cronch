#pragma once
#include <game/event.hpp>
#include <tardigrade/tick_attachment.hpp>
#include <util/common.hpp>

namespace cronch {
using namespace std::chrono_literals;

class AutoPlay;

class Coordinator : public tg::TickAttachment {
	void setup() override;
	void tick(tg::DeltaTime) override;

	void score(event::Score const& score);
	void damage(event::Damage const& damage);
	void reset();
	void demo();

	static constexpr tg::Time restart_delay_v{2s};

	Ptr<AutoPlay> m_auto_play{};
	tg::Time m_dead_elapsed{};
};
} // namespace cronch
