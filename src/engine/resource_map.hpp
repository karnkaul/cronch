#pragma once
#include <ktl/kunique_ptr.hpp>
#include <tardigrade/detail/type_id.hpp>
#include <util/common.hpp>
#include <string>

namespace ROOT_NS {
class ResourceMap {
  public:
	using Uri = std::string;

	ResourceMap();
	ResourceMap(ResourceMap&&) noexcept;
	ResourceMap& operator=(ResourceMap&&) noexcept;
	~ResourceMap() noexcept;

	template <typename Type>
	Ptr<Type> find_or_insert(Uri uri, Type t) {
		if (uri.empty()) { return {}; }
		auto ret = add_base(std::move(uri), ktl::make_unique<Model<Type>>(std::move(t)));
		return &static_cast<Model<Type>*>(ret)->t;
	}

	template <typename Type>
	bool remove(Uri const& uri) {
		return remove(uri, tg::TypeId::make<Type>());
	}

	bool remove(Uri const& uri);

	template <typename Type>
	Ptr<Type> find(Uri const& uri) const {
		if (auto ret = find(uri); ret && ret->type == tg::TypeId::make<Type>()) { return &static_cast<Model<Type>*>(ret)->t; }
		return {};
	}

	template <typename Type>
	Ptr<Type> get(Uri const& uri) const {
		auto ret = find<Type>(uri);
		assert(ret);
		return ret;
	}

	template <typename Type>
	bool contains(Uri const& uri) const {
		return find<Type>(uri) != nullptr;
	}

	bool contains(Uri const& uri) const { return find(uri) != nullptr; }

	std::size_t size() const;
	void clear();

  private:
	struct Base {
		tg::TypeId type{};
		Base(tg::TypeId type) : type(type) {}
		virtual ~Base() = default;
	};

	template <typename T>
	struct Model : Base {
		T t;
		Model(T&& t) : Base{tg::TypeId::make<T>()}, t{std::move(t)} {}
	};

	Ptr<Base> add_base(Uri uri, ktl::kunique_ptr<Base>&& base);
	Ptr<Base> find(Uri const& uri) const;
	bool remove(Uri const& uri, tg::TypeId type);

	struct Impl;
	ktl::kunique_ptr<Impl> m_impl{};
};
} // namespace ROOT_NS
