#include <engine/io.hpp>
#include <ktl/byte_array.hpp>
#include <util/logger.hpp>
#include <filesystem>
#include <fstream>
#include <optional>
#include <ranges>
#include <vector>

namespace ROOT_NS {
namespace {
namespace fs = std::filesystem;

std::vector<std::string> g_prefixes{};

bool read_file(ktl::byte_array& out, fs::path const& path) {
	if (auto in = std::ifstream(path, std::ios::binary | std::ios::ate)) {
		in.unsetf(std::ios::skipws);
		auto const size = in.tellg();
		out.resize_for_overwrite(static_cast<std::size_t>(size));
		in.seekg(0, std::ios::beg);
		in.read(reinterpret_cast<char*>(out.data()), size);
		return true;
	}
	return false;
}
} // namespace

io::Instance::Instance(char const*) {}

io::Instance::~Instance() { g_prefixes.clear(); }

bool io::mount_dir(char const* path) {
	auto const fs_path = fs::absolute(path);
	if (!fs::is_directory(path)) {
		logger::warn("[fs] Cannot mount: [{}], not a directory", path);
		return false;
	}
	auto path_str = fs_path.generic_string();
	if (std::ranges::find(g_prefixes, path_str) != std::ranges::end(g_prefixes)) {
		logger::warn("[fs] Already mounted: [{}]", path);
		return false;
	}
	logger::info("[fs] Mounted [{}]", path);
	g_prefixes.push_back(std::move(path_str));
	return true;
}

bool io::exists(char const* uri) { return !absolute(uri).empty(); }

std::string io::absolute(char const* uri) {
	for (auto const& prefix : g_prefixes) {
		auto const path = fs::path(prefix) / uri;
		if (fs::is_regular_file(path)) { return path.generic_string(); }
	}
	return {};
}

bool io::load(ktl::byte_array& out, char const* uri) {
	for (auto const& prefix : g_prefixes) {
		if (read_file(out, fs::path(prefix) / uri)) {
			logger::debug("[fs] loaded [{}]", uri);
			return true;
		}
	}
	logger::warn("[fs] Failed to open [{}]", uri);
	return false;
}
} // namespace ROOT_NS
