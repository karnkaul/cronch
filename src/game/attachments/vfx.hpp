#pragma once
#include <engine/animated_sprite.hpp>
#include <tardigrade/render_attachment.hpp>
#include <util/common.hpp>
#include <util/pool.hpp>

namespace cronch {
using namespace std::chrono_literals;

class Vfx : public tg::RenderAttachment {
  public:
	glm::vec2 size{50.0f};
	Ptr<vf::Sprite::Sheet const> sheet{};
	tg::Time ttl{1s};

	void spawn(glm::vec2 position, vf::Rgba tint = vf::white_v);

  private:
	void setup() override;
	void tick(tg::DeltaTime dt) override;
	void render(tg::RenderTarget const& target) const override;

	struct Entry {
		AnimatedSprite sprite{};
		tg::Time ttl{};
	};

	struct Factory {
		Entry operator()() const;
	};

	struct {
		util::Pool<Entry, Factory> pool{};
		std::vector<Entry> active{};
	} m_entries{};
};
} // namespace cronch
