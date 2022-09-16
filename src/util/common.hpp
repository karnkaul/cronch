#pragma once
#include <tardigrade/detail/ptr.hpp>

namespace cronch {
template <typename T>
using Ptr = tg::Ptr<T>;

constexpr bool debug_v =
#if defined(IRIS_DEBUG)
	true;
#else
	false;
#endif
} // namespace cronch
