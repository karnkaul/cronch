#pragma once
#include <tardigrade/tick_attachment.hpp>

namespace cronch {
class AutoPlay : public tg::TickAttachment {
  public:
	bool enabled{true};

  private:
	void setup() override {}
	void tick(tg::DeltaTime) override;
};
} // namespace cronch
