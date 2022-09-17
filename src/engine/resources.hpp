#pragma once
#include <engine/resource_map.hpp>
#include <util/index_timeline.hpp>
#include <vulkify/graphics/primitives/sprite.hpp>

namespace capo {
class Sound;
}

namespace vf {
class Ttf;
} // namespace vf

namespace ROOT_NS {
struct Context;

struct SheetAnimation {
	using Sequence = IndexTimeline::Sequence;

	vf::Sprite::Sheet sheet{};
	Sequence sequence{};
};

class Resources {
  public:
	using Uri = ResourceMap::Uri;
	using Sig = std::string_view;

	Resources(Context const& context);

	template <typename Type, std::convertible_to<Uri> T>
	Ptr<Type> load(T uri) {
		if (auto ret = m_map.find<Type>(uri)) { return ret; }
		return do_load<Type>(std::move(uri));
	}

	template <typename Type>
	Ptr<Type> load(Sig sig) {
		return load<Type>(Uri{sig});
	}

	bool unload(Uri const& uri) { return m_map.remove(uri); }

	template <typename Type>
	bool contains(Uri const& uri) const {
		return m_map.contains<Type>(uri);
	}

	std::size_t size() const { return m_map.size(); }
	void clear() { m_map.clear(); }

  private:
	template <typename T>
	using LoadUri = std::string;

	template <typename T>
	Ptr<T> do_load(Uri uri) = delete;

	ResourceMap m_map{};
	Ptr<Context const> m_context{};
};

template <>
Ptr<vf::Texture> Resources::do_load<vf::Texture>(Uri uri);

template <>
Ptr<vf::Ttf> Resources::do_load<vf::Ttf>(Uri uri);

template <>
Ptr<capo::Sound> Resources::do_load<capo::Sound>(Uri uri);

template <>
Ptr<vf::Sprite::Sheet> Resources::do_load<vf::Sprite::Sheet>(Uri uri);
} // namespace ROOT_NS
