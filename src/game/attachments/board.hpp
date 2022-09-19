#pragma once
#include <game/hit_result.hpp>
#include <game/theme.hpp>
#include <tardigrade/render_attachment.hpp>
#include <util/common.hpp>
#include <util/pool.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>
#include <optional>

namespace cronch {
using namespace std::chrono_literals;

class Board : public tg::RenderAttachment {
  public:
	float chomp_speed{100.0f};

	void spawn_food(Lane lane, vf::Radian tumble);
	void spawn_dilator(Lane lane, vf::Radian tumble);
	bool can_hit(Lane lane) const { return !m_entries[lane].empty(); }
	HitResult attempt_hit(Lane lane, vf::Rect const& rect);
	std::optional<vf::Rect> raycast(Lane lane) const;
	bool dilator_enabled() const { return m_dilator.remain > 0s; }
	void dilate_time(float scale, tg::Time duration);
	void reset();

  private:
	struct Chomp {
		glm::vec2 dir{};
		float speed{};
		vf::Radian tumble{};
		ChompType type{};
	};

	struct Dilator {
		tg::Time remain{};

		void enable(float scale, tg::Time ttl);
		void disable();
		void tick(tg::DeltaTime dt);
	};

	struct Entry {
		vf::Sprite sprite{};
		Chomp chomp{};
	};

	struct Factory {
		Entry operator()() const;
	};

	void setup() override;
	void tick(tg::DeltaTime dt) override;
	void render(tg::RenderTarget const& target) const override;

	void prepare(Entry& out, Lane lane, Chomp chomp);
	void release(std::vector<Entry>& out, Ptr<Entry> target);
	std::span<Ptr<Entry>> make_scratch(std::span<Entry> vec);

	util::EnumArray<Lane, std::vector<Entry>> m_entries{};
	util::Pool<Entry, Factory> m_pool{};
	std::vector<Ptr<Entry>> m_scratch{};
	Ptr<vf::Sprite::Sheet const> m_sheet{};
	Theme::Chomps::Data m_data{};
	Dilator m_dilator{};
};
} // namespace cronch
