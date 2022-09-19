#pragma once
#include <game/event.hpp>
#include <ktl/async/kfunction.hpp>
#include <ktl/hash_table.hpp>
#include <tardigrade/attachment.hpp>
#include <util/enum_array.hpp>

namespace cronch {
using OnEvent = ktl::kfunction<void(Event const&)>;

class Dispatch : public tg::Attachment {
  public:
	void attach(tg::Entity::Id id, Event::Type type, OnEvent on_event);
	void clear();
	void dispatch(Event const& event);

  private:
	void setup() override {}

	using Map = ktl::hash_table<tg::Entity::Id, std::vector<OnEvent>>;
	using Array = util::EnumArray<Event::Type, Map>;

	Array m_array{};
	std::vector<OnEvent> m_scratch{};
};
} // namespace cronch
