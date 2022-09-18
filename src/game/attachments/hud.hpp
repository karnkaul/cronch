#pragma once
#include <game/layout.hpp>
#include <tardigrade/render_attachment.hpp>
#include <util/common.hpp>
#include <util/pool.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>
#include <vulkify/graphics/primitives/text.hpp>

namespace cronch {
using namespace std::chrono_literals;

class Hud : public tg::RenderAttachment {
  public:
	tg::Time popup_ttl{1s};
	float popup_y_speed{100.0f};
	vf::Text::Height popup_height{30};

	void popup(std::string text, glm::vec2 position, vf::Rgba tint = vf::white_v);

  private:
	void setup() override;
	void tick(tg::DeltaTime dt) override;
	void render(tg::RenderTarget const& target) const override;

	void update_score(std::int64_t current);

	struct Popup {
		vf::Text text{};
		tg::Time ttl{};
	};

	struct Factory {
		Ptr<vf::Ttf> ttf{};

		Popup operator()() const;
	};

	struct {
		util::Pool<Popup, Factory> pool{};
		std::vector<Popup> active{};
	} m_popups{};
	struct {
		vf::Text text{};
		std::int64_t previous{};
	} m_score{};
	struct {
		std::array<vf::Sprite, layout::max_dilators_v> sprites{};
		int count{};
	} m_dilators{};
};
} // namespace cronch
