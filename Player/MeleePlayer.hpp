#ifndef MELEEPLAYER_HPP
#define MELEEPLAYER_HPP
#include "Player.h"

class MeleePlayer : public Player {
public:
    MeleePlayer(float x, float y);
    void Update(float deltaTime) override;
};
#endif   // MELEEPLAYER_HPP
