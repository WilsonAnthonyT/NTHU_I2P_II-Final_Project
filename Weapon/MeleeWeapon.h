//
// Created by User on 30/05/2025.
//

#ifndef MELEEWEAPON_H
#define MELEEWEAPON_H

#include <iostream>
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "Player/Player.h"
#include "Player/MeleePlayer.hpp"

class MeleeWeapon : public Engine::Sprite {
protected:
    PlayScene *Play;
    Player *player;
    const float RotationRate;
    bool flipped;
    const float speed;
    bool isRotating;

    float rotationProgress;
    float cooldown;
public:
    MeleeWeapon(std::string img, float x, float y, float Rr,Player *player, float speed);
    void Update(float deltaTime) override;
    void Draw() const override;
};

#endif