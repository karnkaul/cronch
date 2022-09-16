#pragma once
#include <vector>

namespace cronch::util {
template <typename Type>
struct PoolFactory {
	Type operator()() const { return Type{}; }
};

template <typename Type, typename Factory = PoolFactory<Type>>
class Pool {
  public:
	using value_type = Type;
	using factory_type = Factory;

	Pool(Factory factory = {}) : m_factory{std::move(factory)} {}

	Type acquire() {
		reserve(1);
		auto ret = std::move(m_reserve.back());
		m_reserve.pop_back();
		return ret;
	}

	void release(Type&& t) { m_reserve.push_back(std::move(t)); }

	void reserve(std::size_t count) {
		m_reserve.reserve(count);
		for (std::size_t i = m_reserve.size(); i < count; ++i) { m_reserve.push_back(m_factory()); }
	}

	std::size_t available() const { return m_reserve.size(); }
	void clear() { m_reserve.clear(); }

	Factory& get_factory() { return m_factory; }
	Factory const& get_factory() const { return m_factory; }

  protected:
	std::vector<Type> m_reserve{};
	Factory m_factory{};
};
} // namespace cronch::util
