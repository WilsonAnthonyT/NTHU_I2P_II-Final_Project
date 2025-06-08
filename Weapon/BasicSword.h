//
// Created by User on 08/06/2025.
//
#include "MeleeWeapon.h"

#ifndef BASICSWORD_H
#define BASICSWORD_H



class Player;
class BasicSword : public MeleeWeapon{
public:
    BasicSword(float x, float y, Player* player);
    // void RotateAnimation(float deltaTime) override;
};



#endif //BASICSWORD_H
