#pragma once
#include <ktl/async/kfunction.hpp>
#include <ktl/hash_table.hpp>
#include <tardigrade/attachment.hpp>
#include <tardigrade/scene.hpp>
#include <typeindex>

namespace cronch {
class Dispatch : public tg::Attachment {
  public:
	template <typename T>
	using OnEvent = ktl::kfunction<void(T const&)>;

	template <typename TEvent>
	void attach(tg::Entity::Id id, OnEvent<TEvent>&& on_event) {
		m_map[key<TEvent>()].push_back({std::move(on_event), id});
	}

	template <typename TEvent>
	void dispatch(TEvent const& event) {
		auto const it = m_map.find(key<TEvent>());
		if (it == m_map.end()) { return; }
		auto temp = std::move(it->second);
		for (auto const& delegate : temp) {
			if (scene()->contains(delegate.id)) { delegate(event); }
		}
		std::move(temp.begin(), temp.end(), std::back_inserter(it->second));
		std::erase_if(it->second, [s = scene()](Delegate const& d) { return !s->contains(d.id); });
	}

	template <typename TEvent>
	void operator()(TEvent const& event) {
		dispatch(event);
	}

  private:
	void setup() override {}

	template <typename T>
	static std::type_index key() {
		return std::type_index(typeid(T));
	}

	struct Delegate {
		using Arg = void const*;
		using Cb = ktl::kfunction<void(Arg)>;

		Cb cb{};
		tg::Entity::Id id{};

		template <typename T>
		Delegate(OnEvent<T>&& on_event, tg::Entity::Id id) : cb{[o = std::move(on_event)](Arg arg) { o(*static_cast<T const*>(arg)); }}, id{id} {}

		template <typename T>
		void operator()(T const& t) const {
			cb(&t);
		}
	};

	ktl::hash_table<std::type_index, std::vector<Delegate>> m_map{};
};
} // namespace cronch
