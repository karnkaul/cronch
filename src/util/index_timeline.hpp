#pragma once
#include <util/common.hpp>
#include <vulkify/core/time.hpp>
#include <vector>

namespace cronch {
using namespace std::chrono_literals;

class IndexTimeline {
  public:
	struct Sequence {
		vf::Time duration{1s};
		std::size_t end{};
		std::size_t begin{};

		constexpr std::size_t length() const { return end - begin; }
	};

	IndexTimeline& set(Sequence sequence);

	void tick(vf::Time dt);
	std::size_t index() const { return m_active.index; }

  protected:
	struct Active {
		std::size_t index{};
		vf::Time remain{};
	};

	Sequence m_sequence{};
	Active m_active{};
};
} // namespace cronch
