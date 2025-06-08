//
// Created by User on 08/06/2025.
//

#ifndef LINKSHIELD_H
#define LINKSHIELD_H
#include "MeleeShield.h"
#include "Engine/Sprite.hpp"


class Player;
class LinkShield : public MeleeShield {
    public:
    LinkShield(float x, float y, Player *player);
};



#endif //LINKSHIELD_H
