#pragma once
#include <game/layout.hpp>
#include <tardigrade/render_attachment.hpp>
#include <util/common.hpp>
#include <util/pool.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>
#include <vulkify/graphics/primitives/text.hpp>
#include <array>

namespace cronch {
using namespace std::chrono_literals;

class Frame;

struct Toast {
	using Tick = void (*)(vf::Text&, tg::DeltaTime, float);

	std::string text{};
	glm::vec2 position{};
	vf::Rgba tint{vf::white_v};
	tg::Time ttl{1s};
	Tick tick{};
};

struct Letterbox {
	enum class State { eDisabling, eDisabled, eEnabling, eEnabled };

	vf::Mesh top{};
	vf::Mesh bottom{};
	vf::Text title{};
	vf::Text subtitle{};
	float speed{750.0f};
	float y_enabled{};
	float y_disabled{};
	bool enabled{true};

	State state() const;
	void toggle();
	void tick(tg::Time dt);
};

class Hud : public tg::RenderAttachment {
  public:
	tg::Time popup_ttl{1s};
	float popup_y_speed{100.0f};
	vf::Text::Height popup_height{30};
	float letterbox_speed{500.0f};
	bool game_over{true};
	bool show_restart{};

	void spawn(Toast toast);
	Letterbox& letterbox() { return m_letterbox; }
	Letterbox const& letterbox() const { return m_letterbox; }

  private:
	void setup() override;
	void tick(tg::DeltaTime dt) override;
	void render(tg::RenderTarget const& target) const override;

	void update_score(std::int64_t current);

	struct Popup {
		vf::Text text{};
		tg::Time elapsed{};
		tg::Time ttl{};
		Toast::Tick tick{};
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
	struct {
		std::array<vf::Sprite, layout::max_health_v> sprites{};
		int count{};
	} m_hearts{};
	Letterbox m_letterbox{};
	struct {
		vf::Text title{};
		vf::Text restart{};
	} m_over{};
};
} // namespace cronch
