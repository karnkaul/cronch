#pragma once
#include <game/attachments/prop.hpp>
#include <game/hit_result.hpp>
#include <game/score.hpp>
#include <game/theme.hpp>
#include <tardigrade/tick_attachment.hpp>
#include <util/cache.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>

namespace cronch {
using namespace std::chrono_literals;

class Controller;

template <typename Type>
class Renderer;

class Player : public tg::TickAttachment {
  public:
	struct Dilation {
		tg::Time duration{3s};
		float scale{0.5f};
	};

	Dilation dilation{};
	Ptr<Controller> controller{};
	Ptr<Renderer<vf::Sprite>> sprite{};
	Ptr<vf::Sprite::Sheet const> sheet{};
	Ptr<Prop> prop{};

	bool can_hit(Lane lane) const;
	HitResult attempt_hit(Lane lane);
	std::optional<vf::Rect> raycast(Lane lane) const;

	void score(glm::vec2 position, ChompType type);
	void reset_multiplier() { m_score.reset_multiplier(); }
	bool try_dilate_time();
	void take_damage(glm::vec2 position);

	Score const& score() const { return m_score; }
	int dilator_count() const { return m_dilators; }

  private:
	void setup() override;
	void tick(tg::DeltaTime dt) override;

	Cache<std::uint32_t> m_uv{};
	Theme::Player::Data m_data{};
	Score m_score{};
	int m_dilators{};
};
} // namespace cronch
