//
// Created by User on 08/06/2025.
//

#ifndef MELEESHIELD_H
#define MELEESHIELD_H
#include "Engine/Sprite.hpp"


class Player;
class MeleeShield : public Engine::Sprite {
    Player *player;
    bool flipped;
    public:
    MeleeShield(std::string img, float x, float y, Player *player);
    void Update(float deltaTime) override;
};



#endif //MELEESHIELD_H
