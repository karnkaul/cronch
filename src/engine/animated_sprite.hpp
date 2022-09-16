#pragma once
#include <util/index_timeline.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>

namespace ROOT_NS {
class AnimatedSprite : public vf::Sprite {
  public:
	using Sequence = IndexTimeline::Sequence;

	AnimatedSprite() = default;
	AnimatedSprite(vf::GfxDevice const& device);

	IndexTimeline timeline{};

	AnimatedSprite& set_sheet(Sprite::Sheet const& sheet, vf::Time duration = {});
	AnimatedSprite& set_sheet(Sprite::Sheet const& sheet, Sequence sequence);
	AnimatedSprite& unset_sheet();

	void tick(vf::Time dt);

  private:
	Ptr<Sprite::Sheet const> m_sheet{};
};
} // namespace ROOT_NS
