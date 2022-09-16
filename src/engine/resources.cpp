#include <engine/animated_sprite.hpp>
#include <engine/context.hpp>
#include <engine/io.hpp>
#include <engine/resources.hpp>
#include <ktl/byte_array.hpp>
#include <util/logger.hpp>
#include <util/property.hpp>
#include <fstream>

namespace ROOT_NS {
namespace {
constexpr vf::Filtering to_filtering(std::string_view str) {
	if (str == "nearest") { return vf::Filtering::eNearest; }
	return vf::Filtering::eLinear;
}

struct SheetInfo {
	std::string texture_uri{};
	glm::ivec2 tile_count{};
	vf::Filtering filtering{vf::Filtering::eLinear};

	explicit operator bool() const { return !texture_uri.empty(); }
};

SheetInfo get_sheet_info(std::istream& in, Ptr<IndexTimeline::Sequence> sequence = {}) {
	auto parser = util::Property::Parser{in};
	auto ret = SheetInfo{};
	parser.parse_all([&](util::Property property) {
		if (property.key == "texture") {
			ret.texture_uri = std::move(property.value);
		} else if (property.key == "x") {
			ret.tile_count.x = std::atoi(property.value.c_str());
		} else if (property.key == "y") {
			ret.tile_count.y = std::atoi(property.value.c_str());
		} else if (property.key == "filtering") {
			ret.filtering = to_filtering(property.value);
		} else if (property.key == "duration" && sequence) {
			sequence->duration = vf::Time(std::atof(property.value.c_str()));
		} else if (sequence && property.key == "begin") {
			auto begin = std::atoi(property.value.c_str());
			if (begin >= 0) { sequence->begin = static_cast<std::size_t>(begin); }
		} else if (sequence && property.key == "end") {
			auto end = std::atoi(property.value.c_str());
			if (end >= 0) { sequence->end = static_cast<std::size_t>(end); }
		}
	});
	return ret;
}

bool validate(SheetInfo const& info, std::string_view const uri) {
	if (info.texture_uri.empty()) {
		logger::warn("[Resources] Required field missing/empty in Sprite::Sheet [{}]: [texture]", uri);
		return false;
	}
	if (info.tile_count.x <= 0 || info.tile_count.y <= 0) {
		logger::warn("[Resources] Required field missing/empty in Sprite::Sheet [{}]: [x/y]", uri);
		return false;
	}
	return true;
}

[[maybe_unused]] bool validate(IndexTimeline::Sequence const& sequence, std::string_view const uri) {
	if (sequence.duration < 0s) {
		logger::warn("[Resources] Invalid duration in Sprite::Sheet [{}]: [{}]", uri, sequence.duration.count());
		return false;
	}
	if (sequence.begin > sequence.end) {
		logger::warn("[Resources] Invalid sequence in Sprite::Sheet [{}]: [{}] - [{}]", sequence.begin, sequence.end);
		return false;
	}
	return true;
}

bool load_image(ktl::byte_array& out_buffer, vf::Image& out, char const* uri) {
	if (!io::load(out_buffer, uri)) {
		logger::warn("[Resources] Failed to read image: [{}]", uri);
		return false;
	}
	if (!out.load(vf::Image::Encoded{out_buffer})) {
		logger::warn("[Resources] Failed to open image: [{}]", uri);
		return false;
	}
	return true;
}

struct Stopwatch {
	vf::Clock::time_point start{vf::Clock::now()};

	vf::Time dt() const { return vf::diff(start); }
};
} // namespace

Resources::Resources(Context const& context) : m_context(&context) {}

// template <>
// SheetInfo Resources::do_load<SheetInfo>(LoadInfo<SheetInfo> const& info) const {
// 	auto sw = Stopwatch{};
// 	auto buffer = ktl::byte_array{};
// 	if (!io::load(buffer, info.uri.c_str())) {
// 		logger::warn("[Resources] Failed to read sheet animation: [{}]", info.uri);
// 		return {};
// 	}
// 	auto str = std::stringstream{std::string{reinterpret_cast<char const*>(buffer.data()), buffer.size()}};
// 	auto const ret = get_sheet_info(str);
// 	if (!validate(ret, info.uri)) { return {}; }
// 	logger::debug("[Resources] SheetInfo loaded: [{}] in {:.1f}ms", info.uri, sw.dt().count() * 1000.0f);
// 	return ret;
// }

template <>
Ptr<vf::Texture> Resources::do_load<vf::Texture>(Uri uri) {
	auto sw = Stopwatch{};
	auto image = vf::Image{};
	auto buffer = ktl::byte_array{};
	if (!load_image(buffer, image, uri.data())) {
		logger::warn("[Resources] Failed to open Image: [{}]", uri);
		return {};
	}
	auto texture = vf::Texture{
		m_context->vf_context.device(),
		image,
		vf::TextureCreateInfo{.filtering = vf::Filtering::eLinear},
	};
	if (!texture) {
		logger::warn("[Resources] Failed to create Texture: [{}]", uri);
		return {};
	}
	logger::debug("[Resources] Texture loaded: [{}] in {:.1f}ms", uri, sw.dt().count() * 1000.0f);
	auto ret = m_map.find_or_insert(std::move(uri), std::move(texture));
	return ret;
}

template <>
Ptr<vf::Ttf> Resources::do_load<vf::Ttf>(Uri uri) {
	auto sw = Stopwatch{};
	auto buffer = ktl::byte_array{};
	if (!io::load(buffer, uri.data())) {
		logger::warn("[Resources] Failed to open font: [{}]", uri);
		return {};
	}
	auto ttf = vf::Ttf{m_context->vf_context.device()};
	if (!ttf.load(buffer)) {
		logger::warn("[Resources] Failed to create Ttf: [{}]", uri);
		return {};
	}
	logger::debug("[Resources] Ttf loaded: [{}] in {:.1f}ms", uri, sw.dt().count() * 1000.0f);
	auto ret = m_map.find_or_insert(std::move(uri), std::move(ttf));
	return ret;
}

template <>
Ptr<capo::Sound> Resources::do_load<capo::Sound>(Uri uri) {
	auto sw = Stopwatch{};
	auto buffer = ktl::byte_array{};
	if (!io::load(buffer, uri.data())) {
		logger::warn("[Resources] Failed to read PCM: [{}]", uri);
		return {};
	}
	auto pcm = capo::PCM::from_memory(buffer, capo::FileFormat::eUnknown);
	if (!pcm) {
		logger::warn("[Resources] Failed to load PCM: [{}]", uri);
		return {};
	}
	auto sound = m_context->capo_instance->make_sound(*pcm);
	logger::debug("[Resources] Sound loaded: [{}] in {:.1f}ms", uri, sw.dt().count() * 1000.0f);
	auto ret = m_map.find_or_insert(std::move(uri), std::move(sound));
	return ret;
}

template <>
Ptr<vf::Sprite::Sheet> Resources::do_load<vf::Sprite::Sheet>(Uri uri) {
	auto sw = Stopwatch{};
	auto buffer = ktl::byte_array{};
	if (!io::load(buffer, uri.data())) {
		logger::warn("[Resources] Failed to open sprite sheet: [{}]", uri);
		return {};
	}
	auto str = std::stringstream{std::string{reinterpret_cast<char const*>(buffer.data()), buffer.size()}};
	auto const sheet_info = get_sheet_info(str, {});
	if (!validate(sheet_info, uri)) { return {}; }

	auto* texture = load<vf::Texture>(sheet_info.texture_uri);
	if (!texture) {
		logger::warn("[Resources] Failed to load SpriteSheet [{}] texture: [{}]", sheet_info.texture_uri);
		return {};
	}

	auto sheet = vf::Sprite::Sheet{};
	auto const tile_count = glm::uvec2{sheet_info.tile_count};
	sheet.set_uvs(texture, tile_count.y, tile_count.x);
	logger::debug("[Resources] SpriteSheet loaded: [{}] in {:.1f}ms", uri, sw.dt().count() * 1000.0f);
	auto ret = m_map.find_or_insert(std::move(uri), std::move(sheet));
	return ret;
}

// template <>
// SheetAnimation Resources::do_load<SheetAnimation>(LoadInfo<SheetAnimation> const& info) const {
// 	auto sw = Stopwatch{};
// 	auto buffer = ktl::byte_array{};
// 	if (!io::load(buffer, info.uri.c_str())) {
// 		logger::warn("[Resources] Failed to read sheet animation: [{}]", info.uri);
// 		return {};
// 	}
// 	auto str = std::stringstream{std::string{reinterpret_cast<char const*>(buffer.data()), buffer.size()}};
// 	auto ret = SheetAnimation{};
// 	auto const sheet_info = get_sheet_info(str, &ret.sequence);
// 	if (!validate(sheet_info, info.uri)) { return {}; }
// 	if (!validate(ret.sequence, info.uri)) { return {}; }

// 	auto image = vf::Image{};
// 	if (!load_image(buffer, image, sheet_info.texture_uri.c_str())) { return {}; }
// 	auto const tci = vf::TextureCreateInfo{.filtering = sheet_info.filtering};
// 	ret.texture = vf::Texture{context.vf_context.device(), image, tci};
// 	auto const tile_count = glm::uvec2{sheet_info.tile_count};
// 	ret.sheet.set_uvs(&ret.texture, tile_count.y, tile_count.x);
// 	if (ret.sequence.end <= ret.sequence.begin) { ret.sequence.end = ret.sheet.uv_count(); }

// 	logger::debug("[Resources] SheetAnimation loaded: [{}] in {:.1f}ms", info.uri, sw.dt().count() * 1000.0f);
// 	return ret;
// }
} // namespace ROOT_NS
