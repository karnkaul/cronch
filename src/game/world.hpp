#pragma once
#include <tardigrade/scene.hpp>
#include <util/common.hpp>

namespace cronch {
class Player;
class Chomper;

class World : public tg::Scene {
  public:
	Ptr<Player> player{};
	Ptr<Chomper> chomper{};

  private:
	void setup() override;
};
} // namespace cronch
