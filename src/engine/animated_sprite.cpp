#include <engine/animated_sprite.hpp>

namespace ROOT_NS {
AnimatedSprite::AnimatedSprite(vf::GfxDevice const& device) : Sprite(device) { draw_invalid = true; }

AnimatedSprite& AnimatedSprite::set_sheet(Sprite::Sheet const& sheet, vf::Time const duration) { return set_sheet(sheet, {duration, sheet.uv_count()}); }

AnimatedSprite& AnimatedSprite::set_sheet(Sprite::Sheet const& sheet, Sequence sequence) {
	Sprite::set_sheet(&sheet);
	m_sheet = &sheet;
	timeline.set(sequence);
	return *this;
}

void AnimatedSprite::tick(vf::Time dt) {
	timeline.tick(dt);
	if (m_sheet) { Sprite::set_sheet(m_sheet, static_cast<UvIndex>(timeline.index())); }
}

AnimatedSprite& AnimatedSprite::unset_sheet() {
	Sprite::set_sheet({});
	timeline = {};
	return *this;
}
} // namespace ROOT_NS
