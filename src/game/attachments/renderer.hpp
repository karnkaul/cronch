#pragma once
#include <engine/frame.hpp>
#include <game/attachments/prop.hpp>
#include <tardigrade/render_attachment.hpp>
#include <tardigrade/services.hpp>
#include <vulkify/graphics/primitives/mesh.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>

namespace cronch {
template <std::derived_from<vf::Primitive> Type>
class TRenderer : public tg::RenderAttachment {
  public:
	Type& get() { return m_t; }
	Type const& get() const { return m_t; }

  protected:
	void setup() override {
		tg::RenderAttachment::setup();
		m_t = Type{*tg::locate<vf::GfxDevice const*>()};
	}

	void render(tg::RenderTarget const& target) const override {
		auto const& frame = static_cast<Frame const&>(target);
		frame.draw(m_t);
	}

	Type m_t{};
};

template <typename Type>
class Renderer : public TRenderer<Type> {};

template <typename Type>
	requires(std::derived_from<Type, vf::Mesh>)
class Renderer<Type> : public TRenderer<Type> {
  protected:
	void tick(tg::DeltaTime) override {
		if (auto const* prop = this->entity()->template find<Prop>()) { this->get().instance().transform = prop->transform; }
	}
};

template <typename Type>
	requires(std::derived_from<Type, vf::Sprite>)
class Renderer<Type> : public TRenderer<Type> {
  protected:
	void tick(tg::DeltaTime) override {
		if (auto const* prop = this->entity()->template find<Prop>()) {
			this->get().transform() = prop->transform;
			if (prop->bounds != this->get().size()) { this->get().set_size(prop->bounds); }
		}
	}
};
} // namespace cronch
