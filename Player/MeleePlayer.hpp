#ifndef MELEEPLAYER_HPP
#define MELEEPLAYER_HPP
#include "Player.h"
#include "Weapon/MeleeShield.h"
#include "Weapon/RangeWeapon.h"

class MeleeWeapon;
class LinkWeapon;
class MeleePlayer : public Player {
public:
    MeleePlayer(float x, float y);
    void Update(float deltaTime) override;
    MeleeWeapon *weapon;
    MeleeShield *shield;
    RangeWeapon *range;

    void ChangeWeapon(MeleeWeapon * newWeapon);
    void ChangeShield(MeleeShield * newShield);
};
#endif   // MELEEPLAYER_HPP
