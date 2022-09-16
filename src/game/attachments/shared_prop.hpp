#pragma once
#include <game/attachments/prop.hpp>
#include <tardigrade/tick_attachment.hpp>

namespace cronch {
class SharedProp : public tg::TickAttachment {
  public:
	Ptr<Prop> prop{};

	void setup() override { prop = entity()->find_or_attach<Prop>(); }
};
} // namespace cronch
