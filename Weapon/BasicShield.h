//
// Created by User on 08/06/2025.
//

#ifndef BASICSHIELD_H
#define BASICSHIELD_H
#include "MeleeShield.h"
#include "Engine/Sprite.hpp"

class Player;
class BasicShield : public MeleeShield{
    public:
    BasicShield(float x, float y, Player *player);
};



#endif //BASICSHIELD_H
