#pragma once
#include <string>

namespace ktl {
class byte_array;
}

namespace ROOT_NS {
namespace io {
class Instance {
  public:
	Instance(char const* arg0);
	~Instance();

	Instance& operator=(Instance&&) = delete;
};

bool mount_dir(char const* path);
bool exists(char const* uri);
std::string absolute(char const* uri);
bool load(ktl::byte_array& out, char const* uri);
} // namespace io
} // namespace ROOT_NS
