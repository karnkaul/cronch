#pragma once
#include <capo/capo.hpp>
#include <vulkify/vulkify.hpp>

namespace ROOT_NS {
struct Context {
	vf::Context vf_context;
	ktl::kunique_ptr<capo::Instance> capo_instance{};
};
} // namespace ROOT_NS
