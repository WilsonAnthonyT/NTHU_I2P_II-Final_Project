//
// Created by User on 08/06/2025.
//

#include "MeleeShield.h"
#include <string>

#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"

MeleeShield::MeleeShield(std::string img, float x, float y, Player *player) : Sprite(img, x, y), player(player), flipped(false) {
}

void MeleeShield::Update(float deltaTime){
    if (player->Visible) this->Visible = true;
    else this->Visible = false;

    float halfSize_x = abs(player->Size.x) / 2;
    float halfSize_y = abs(player->Size.y) / 2;
    Position.y= player->Position.y + halfSize_y;
    flipped=player->flipped;
    if (flipped) {
        Size.x = fabs(Size.x);
        Position.x= player->Position.x-halfSize_x*2.0f;
    }else {
        Size.x = -fabs(Size.x);
        Position.x=player->Position.x+halfSize_x*2.0f;
    }
}