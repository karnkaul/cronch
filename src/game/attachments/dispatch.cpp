#include <game/attachments/dispatch.hpp>
#include <tardigrade/scene.hpp>

namespace cronch {
void Dispatch::attach(tg::Entity::Id id, Event::Type type, OnEvent on_event) {
	if (!on_event) { return; }
	m_array[type][id].push_back(std::move(on_event));
}

void Dispatch::clear() {
	for (auto& map : m_array.array) { map.clear(); }
}

void Dispatch::dispatch(Event const& event) {
	assert(event.type < Event::Type::eCOUNT_);
	auto& map = m_array[event.type];
	for (auto it = map.begin(); it != map.end();) {
		auto const& [id, vec] = *it;
		if (!scene()->contains(id)) {
			it = map.erase(it);
			continue;
		}
		auto temp = std::move(vec);
		for (auto const& callback : temp) { callback(event); }
		std::move(temp.begin(), temp.end(), std::back_inserter(vec));
		++it;
	}
}
} // namespace cronch
