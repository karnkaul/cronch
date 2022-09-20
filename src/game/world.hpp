#pragma once
#include <tardigrade/scene.hpp>
#include <util/common.hpp>

namespace cronch {
class Player;
class Board;
class Hud;
class Vfx;
class Coordinator;
class Dispatch;
class WaveGen;

class World : public tg::Scene {
  public:
	Ptr<Player> player{};
	Ptr<Board> board{};
	Ptr<Hud> hud{};
	Ptr<Vfx> puff{};
	Ptr<Coordinator> coordinator{};
	Ptr<Dispatch> dispatch{};
	Ptr<WaveGen> wave_gen{};

  private:
	void setup() override;
};
} // namespace cronch
