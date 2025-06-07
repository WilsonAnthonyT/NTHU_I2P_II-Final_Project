//
// Created by User on 08/06/2025.
//
#include "MeleeWeapon.h"

#ifndef LINKWEAPON_H
#define LINKWEAPON_H


class Player;
class LinkWeapon : public MeleeWeapon{
    public:
    LinkWeapon(float x, float y, Player* player);
    // void RotateAnimation(float deltaTime) override;
};



#endif //LINKWEAPON_H
