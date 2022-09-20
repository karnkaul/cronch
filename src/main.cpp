#include <engine/context.hpp>
#include <engine/frame.hpp>
#include <engine/io.hpp>
#include <engine/resources.hpp>
#include <game/attachments/board.hpp>
#include <game/attachments/controller.hpp>
#include <game/attachments/dispatch.hpp>
#include <game/attachments/hud.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/vfx.hpp>
#include <game/attachments/wave.hpp>
#include <game/event.hpp>
#include <game/layout.hpp>
#include <game/theme.hpp>
#include <game/world.hpp>
#include <glm/gtx/norm.hpp>
#include <tardigrade/tardigrade.hpp>
#include <util/logger.hpp>
#include <util/random.hpp>
#include <vulkify/instance/mouse.hpp>
#include <filesystem>

namespace cronch {
namespace fs = std::filesystem;

namespace {
struct AutoPlay : tg::TickAttachment {
	bool enabled{};

	void setup() override {}

	void tick(tg::DeltaTime) override {
		if (!enabled) { return; }
		auto* world = static_cast<World*>(scene());
		if (world->player->controller->state() != Controller::State::eIdle) { return; }
		struct {
			vf::Rect rect{};
			Lane lane{};
			float sqr_dist{};
			bool valid{};
		} target{};
		for (Lane l = Lane{}; l < Lane::eCOUNT_; l = increment(l)) {
			auto const closest = world->board->raycast(l);
			if (!closest) { continue; }
			auto const sqr_dist = glm::length2(world->player->prop->transform.position - closest->offset);
			if (!target.valid || sqr_dist < target.sqr_dist) { target = {*closest, l, sqr_dist, true}; }
		}
		if (!target.valid || !world->player->controller->can_chomp(target.rect)) { return; }

		world->player->controller->push(target.lane);
	}
};

struct Debug : tg::TickAttachment {
	Ptr<AutoPlay> auto_play{};

	void setup() override { auto_play = entity()->attach<AutoPlay>(); }

	void tick(tg::DeltaTime) override {
		auto* world = static_cast<World*>(scene());
		using vf::keyboard::held;
		using vf::keyboard::released;
		if (released(vf::Key::eEscape)) { tg::locate<vf::Context*>()->close(); }
		if (released(vf::Key::eT)) {
			static constexpr auto lanes_v = std::array{Lane::eLeft, Lane::eUp, Lane::eRight, Lane::eDown};
			auto const lane = lanes_v[util::random_range(0UL, std::size(lanes_v) - 1)];
			auto const tumble = vf::Degree{util::random_range(-180.0f, 180.0f)};
			if (held(vf::Key::eLeftControl) || held(vf::Key::eRightControl)) {
				world->board->spawn_dilator(lane, tumble);
			} else {
				world->board->spawn_food(lane, tumble);
			}
		}

		if (released(vf::Key::eR)) { world->dispatch->dispatch(Event::Reset{}); }
		if (released(vf::Key::eP)) { auto_play->enabled = !auto_play->enabled; }
		if (released(vf::Key::eO)) { world->wave_gen->advance(); }
	}
};

fs::path find_data(fs::path start) {
	while (!start.empty() && start.parent_path() != start) {
		auto ret = start / "data";
		if (fs::is_directory(ret)) { return ret; }
		start = start.parent_path();
	}
	return {};
}

std::optional<Context> make_context(char const* arg0) {
	auto builder = vf::Builder{};
	builder.set_extent(layout::extent).set_flag(vf::WindowFlag::eResizable);
	auto vf = builder.build();
	if (!vf) {
		logger::error("Failed to create vulkify instance");
		return {};
	}
	vf->camera().view.set_extent(layout::extent);
	vf->lock_aspect_ratio(true);
	auto ret = Context{std::move(*vf)};

	io::mount_dir(find_data(arg0).string().c_str());
	ret.capo_instance = capo::Instance::make();
	return ret;
}

Theme load_theme(Resources& out) {
	auto ret = Theme{};
	if (auto loaded = ret.load("theme.txt")) {
		logger::info("[Theme] loaded [{}] entries", loaded);
		out.load<vf::Sprite::Sheet>(ret.player.assets.sheet);
		out.load<vf::Sprite::Sheet>(ret.vfx.puff.sheet);
		auto* sheet = out.load<vf::Sprite::Sheet>(ret.chomps.assets.sheet);
		if (ret.chomps.data.uvs.last == 0) { ret.chomps.data.uvs.last = static_cast<std::uint32_t>(sheet->uv_count()); }
	}
	return ret;
}
} // namespace
} // namespace cronch

int main(int, char** argv) {
	using namespace cronch;

	auto result = make_context(argv[0]);
	if (!result) { return EXIT_FAILURE; }

	auto services = tg::Services::Scoped{};
	auto context = tg::ServiceProvider<Context>{std::move(*result)};
	auto resources = tg::ServiceProvider<Resources>{context};
	auto theme = tg::ServiceProvider<Theme>{load_theme(resources)};

	tg::Services::provide(&context.vf_context);
	tg::Services::provide(&context.vf_context.device());
	auto io_instance = io::Instance{argv[0]};

	auto director = tg::Director{};
	tg::Services::provide(&director);
	auto& world = director.enqueue<World>();

	world.spawn<Debug>();

	context.vf_context.show();
	while (!context.vf_context.closing()) {
		auto frame = Frame{context.vf_context.frame()};
		director.tick(frame.dt());
		director.render(frame);
	}
}
