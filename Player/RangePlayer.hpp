#ifndef RANGEPLAYER_HPP
#define RANGEPLAYER_HPP
#include "Player.h"
#include "Weapon/RangeWeapon.h"

class RangePlayer : public Player {

public:
    RangePlayer(float x, float y);
    void Update(float deltaTime) override;
    void UpdateAnimation(float deltaTime);
    RangeWeapon *weapon;
};
#endif   // RANGEPLAYER_HPP
