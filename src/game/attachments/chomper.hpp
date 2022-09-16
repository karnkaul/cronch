#pragma once
#include <game/lane.hpp>
#include <tardigrade/render_attachment.hpp>
#include <util/common.hpp>
#include <util/pool.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>

namespace cronch {
class Chomper : public tg::RenderAttachment {
  public:
	static constexpr float speed_v{100.0f};

	void spawn(Lane lane, vf::Radian tumble, float speed = speed_v);
	bool has_chomper(Lane lane) const { return !m_entries[lane].empty(); }
	bool did_score(Lane lane);
	bool did_hit();

  private:
	struct Instance {
		vf::Sprite sprite{};
		glm::vec2 dir{};
		float speed{};
		vf::Radian tumble{};
	};

	struct Factory {
		Instance operator()() const;
	};

	void setup() override {}
	void tick(tg::DeltaTime dt) override;
	void render(tg::RenderTarget const& target) const override;

	void prepare(Instance& out, Lane lane, vf::Radian tumble, float speed);
	void release(std::vector<Instance>& out, Ptr<Instance> target);
	std::span<Ptr<Instance>> make_scratch(std::span<Instance> vec);

	util::EnumArray<Lane, std::vector<Instance>> m_entries{};
	util::Pool<Instance, Factory> m_pool{};
	std::vector<Ptr<Instance>> m_scratch{};
};
} // namespace cronch
