#include <engine/io.hpp>
#include <game/theme.hpp>
#include <ktl/byte_array.hpp>
#include <util/logger.hpp>
#include <util/property.hpp>
#include <sstream>

namespace cronch {
namespace {
glm::vec2 get_vec2(std::string value) {
	auto const x = value.find('x');
	if (x == std::string::npos) { return {}; }
	auto ret = glm::vec2{};
	ret.x = static_cast<float>(std::atof(value.substr(0, x).c_str()));
	ret.y = static_cast<float>(std::atof(value.substr(x + 1).c_str()));
	return ret;
}

struct ThemeParser {
	Theme& out;
	std::size_t parsed{};

	std::string_view full_key{};
	std::string_view sub_key{};
	std::string value{};

	void error() const { logger::warn("[Theme] Unrecognized property: {}", full_key); }

	bool match(std::string_view const expected) {
		if (sub_key.starts_with(expected)) {
			sub_key = sub_key.substr(expected.size());
			return true;
		}
		return false;
	}

	bool terminal(std::string_view const expected) {
		if (match(expected) && sub_key.empty()) {
			++parsed;
			return true;
		}
		return false;
	}

	void player_assets() {
		if (terminal("sheet")) {
			out.player.assets.sheet = std::move(value);
			return;
		}
		error();
	}

	void player_uvs() {
		if (terminal("idle")) {
			out.player.data.uvs.idle = static_cast<std::uint32_t>(std::atoi(value.c_str()));
			return;
		}
		if (terminal("attack")) {
			out.player.data.uvs.attack = static_cast<std::uint32_t>(std::atoi(value.c_str()));
			return;
		}
		error();
	}

	void player_data() {
		if (match("uv.")) { return player_uvs(); }
		error();
	}

	void player() {
		if (match("assets.")) { return player_assets(); }
		if (match("data.")) { return player_data(); }
		error();
	}

	void chomp_assets() {
		if (terminal("sheet")) {
			out.chomps.assets.sheet = std::move(value);
			return;
		}
		error();
	}

	void chomp_uvs() {
		if (terminal("first")) {
			out.chomps.data.uvs.first = static_cast<std::uint32_t>(std::atoi(value.c_str()));
			return;
		}
		if (terminal("last")) {
			out.chomps.data.uvs.last = static_cast<std::uint32_t>(std::atoi(value.c_str()));
			return;
		}
		if (terminal("dilator")) {
			out.chomps.data.uvs.dilator = static_cast<std::uint32_t>(std::atoi(value.c_str()));
			return;
		}
		error();
	}

	void chomp_data() {
		if (match("uv.")) { return chomp_uvs(); }
		error();
	}

	void chomp() {
		if (match("assets.")) { return chomp_assets(); }
		if (match("data.")) { return chomp_data(); }
		error();
	}

	void hud_assets() {
		if (terminal("font")) {
			out.hud.assets.font = std::move(value);
			return;
		}
		error();
	}

	void hud_data() {
		if (terminal("score_height")) {
			out.hud.data.score_height = static_cast<std::uint32_t>(std::atoi(value.c_str()));
			return;
		}
		error();
	}

	void hud() {
		if (match("assets.")) { return hud_assets(); }
		if (match("data.")) { return hud_data(); }
		error();
	}

	void vfx_puff() {
		if (terminal("sheet")) {
			out.vfx.puff.sheet = std::move(value);
			return;
		}
		if (terminal("duration")) {
			out.vfx.puff.duration = static_cast<float>(std::atof(value.c_str()));
			return;
		}
		error();
	}

	void vfx() {
		if (match("puff.")) { return vfx_puff(); }
		error();
	}

	std::size_t operator()(std::istream& in) {
		auto parser = util::Property::Parser{in};
		parser.parse_all([this](util::Property p) {
			full_key = sub_key = p.key;
			value = std::move(p.value);
			if (match("player.")) { return player(); }
			if (match("chomp.")) { return chomp(); }
			if (match("hud.")) { return hud(); }
			if (match("vfx.")) { return vfx(); }
			error();
		});
		return parsed;
	}
};
} // namespace

std::size_t Theme::load(char const* uri) {
	auto buffer = ktl::byte_array{};
	if (!io::load(buffer, uri)) { return {}; }
	auto str = std::stringstream{std::string{reinterpret_cast<char const*>(buffer.data()), buffer.size()}};
	return ThemeParser{*this}(str);
}
} // namespace cronch
