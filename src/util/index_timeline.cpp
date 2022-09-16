#include <util/index_timeline.hpp>
#include <algorithm>
#include <numeric>

namespace cronch {
IndexTimeline& IndexTimeline::set(Sequence sequence) {
	m_sequence = sequence;
	m_active.index = sequence.begin;
	auto const length = m_sequence.length();
	m_active.remain = length > 0 ? m_sequence.duration / length : 0s;
	return *this;
}

void IndexTimeline::tick(vf::Time dt) {
	auto const length = m_sequence.length();
	if (m_active.index >= length) { return; }
	m_active.remain -= dt;
	if (m_active.remain <= 0s) {
		if (++m_active.index >= m_sequence.end) { m_active.index = m_sequence.begin; }
		m_active.remain = m_sequence.duration / length;
	}
}
} // namespace cronch
