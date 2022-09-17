#pragma once
#include <game/attachments/prop.hpp>
#include <tardigrade/tick_attachment.hpp>

namespace cronch {
using namespace std::chrono_literals;

class SharedProp : public tg::TickAttachment {
  public:
	Ptr<Prop> prop{};

	void setup() override { prop = entity()->find_or_attach<Prop>(); }
};
} // namespace cronch
