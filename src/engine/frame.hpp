#pragma once
#include <tardigrade/render_target.hpp>
#include <vulkify/context/frame.hpp>

namespace ROOT_NS {
struct Frame : vf::Frame, tg::RenderTarget {
	Frame(vf::Frame&& frame) { static_cast<vf::Frame&>(*this) = std::move(frame); }
};
} // namespace ROOT_NS
