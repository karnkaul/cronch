#pragma once
#include <tardigrade/attachment.hpp>
#include <util/common.hpp>
#include <vulkify/core/rect.hpp>
#include <vulkify/core/transform.hpp>

namespace cronch {
class Prop : public tg::Attachment {
  public:
	vf::Transform transform{};
	glm::vec2 bounds{};

	vf::Rect rect() const { return {{bounds, transform.position}}; }

	void setup() override {}
};
} // namespace cronch
