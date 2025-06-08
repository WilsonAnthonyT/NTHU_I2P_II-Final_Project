#ifndef RANGEPLAYER_HPP
#define RANGEPLAYER_HPP
#include "Player.h"

class RangePlayer : public Player {
public:
    RangePlayer(float x, float y);
    void Update(float deltaTime) override;
};
#endif   // RANGEPLAYER_HPP
