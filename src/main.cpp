#include <engine/context.hpp>
#include <engine/frame.hpp>
#include <engine/io.hpp>
#include <engine/resources.hpp>
#include <game/attachments/board.hpp>
#include <game/attachments/player.hpp>
#include <game/attachments/renderer.hpp>
#include <game/layout.hpp>
#include <game/world.hpp>
#include <tardigrade/tardigrade.hpp>
#include <util/logger.hpp>
#include <vulkify/instance/mouse.hpp>
#include <filesystem>
#include <random>

namespace cronch {
namespace fs = std::filesystem;

namespace {
template <typename T>
T random_range(T const lo, T const hi) {
	static auto eng = std::default_random_engine{std::random_device{}()};
	if constexpr (std::integral<T>) {
		return std::uniform_int_distribution<T>{lo, hi}(eng);
	} else {
		return std::uniform_real_distribution<T>{lo, hi}(eng);
	}
}

struct Debug : tg::TickAttachment {
	void setup() override {}
	void tick(tg::DeltaTime) override {
		auto* world = static_cast<World*>(scene());
		using vf::keyboard::held;
		using vf::keyboard::pressed;
		if (pressed(vf::Key::eEscape)) { tg::locate<vf::Context*>()->close(); }
		if (pressed(vf::Key::eP)) {
			static constexpr auto lanes_v = std::array{Lane::eLeft, Lane::eUp, Lane::eRight, Lane::eDown};
			auto const lane = static_cast<Lane>(random_range(0UL, std::size(lanes_v) - 1));
			auto const tumble = vf::Degree{random_range(-180.0f, 180.0f)};
			if (held(vf::Key::eLeftControl) || held(vf::Key::eRightControl)) {
				world->board->spawn_dilator(lane, tumble);
			} else {
				world->board->spawn_food(lane, tumble);
			}
		}
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
} // namespace
} // namespace cronch

int main(int, char** argv) {
	using namespace cronch;

	auto result = make_context(argv[0]);
	if (!result) { return EXIT_FAILURE; }

	auto services = tg::Services::Scoped{};
	auto context = tg::ServiceProvider<Context>{std::move(*result)};
	auto resources = tg::ServiceProvider<Resources>{context};
	tg::Services::provide(&context.vf_context);
	tg::Services::provide(&context.vf_context.device());
	auto io_instance = io::Instance{argv[0]};

	auto* sheet = resources.load<vf::Sprite::Sheet>("textures/test_sheet.txt");

	auto director = tg::Director{};
	tg::Services::provide(&director);
	auto& world = director.enqueue<World>();

	world.spawn<Debug>();

	world.player->sheet = sheet;
	world.player->sprite->get().set_sheet(sheet);

	context.vf_context.show();
	while (!context.vf_context.closing()) {
		auto frame = Frame{context.vf_context.frame()};
		director.tick(frame.dt());
		director.render(frame);
	}
}
