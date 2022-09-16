#include <engine/resource_map.hpp>
#include <ktl/hash_table.hpp>
#include <mutex>

namespace ROOT_NS {
struct ResourceMap::Impl {
	ktl::hash_table<Uri, ktl::kunique_ptr<Base>> map{};
	std::mutex mutex{};
};

ResourceMap::ResourceMap() : m_impl(ktl::make_unique<Impl>()) {}

ResourceMap::ResourceMap(ResourceMap&&) noexcept = default;
ResourceMap& ResourceMap::operator=(ResourceMap&&) noexcept = default;
ResourceMap::~ResourceMap() noexcept = default;

bool ResourceMap::remove(Uri const& uri) {
	auto lock = std::scoped_lock(m_impl->mutex);
	if (auto it = m_impl->map.find(uri); it != m_impl->map.end()) {
		m_impl->map.erase(it);
		return true;
	}
	return false;
}

std::size_t ResourceMap::size() const {
	auto lock = std::scoped_lock(m_impl->mutex);
	return m_impl->map.size();
}

void ResourceMap::clear() {
	auto lock = std::scoped_lock(m_impl->mutex);
	m_impl->map.clear();
}

Ptr<ResourceMap::Base> ResourceMap::add_base(Uri uri, ktl::kunique_ptr<Base>&& base) {
	auto lock = std::scoped_lock(m_impl->mutex);
	if (auto it = m_impl->map.find(uri); it != m_impl->map.end()) { return it->second.get(); }
	auto [it, _] = m_impl->map.insert_or_assign(std::move(uri), std::move(base));
	return it->second.get();
}

auto ResourceMap::find(Uri const& uri) const -> Ptr<Base> {
	auto lock = std::scoped_lock(m_impl->mutex);
	if (auto it = m_impl->map.find(uri); it != m_impl->map.end()) { return it->second.get(); }
	return {};
}

bool ResourceMap::remove(Uri const& uri, tg::TypeId type) {
	auto lock = std::scoped_lock(m_impl->mutex);
	if (auto it = m_impl->map.find(uri); it != m_impl->map.end() && it->second->type == type) {
		m_impl->map.erase(it);
		return true;
	}
	return false;
}
} // namespace ROOT_NS
