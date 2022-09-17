#pragma once
#include <tardigrade/scene.hpp>
#include <util/common.hpp>

namespace cronch {
class Player;
class Board;
class Hud;
class Vfx;

class World : public tg::Scene {
  public:
	Ptr<Player> player{};
	Ptr<Board> board{};
	Ptr<Hud> hud{};
	Ptr<Vfx> puff{};

  private:
	void setup() override;
};
} // namespace cronch
