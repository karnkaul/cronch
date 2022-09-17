#pragma once
#include <tardigrade/scene.hpp>
#include <util/common.hpp>

namespace cronch {
class Player;
class Board;

class World : public tg::Scene {
  public:
	Ptr<Player> player{};
	Ptr<Board> board{};

  private:
	void setup() override;
};
} // namespace cronch
