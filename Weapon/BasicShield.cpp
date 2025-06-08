//
// Created by User on 08/06/2025.
//

#include "BasicShield.h"
#include "Scene/PlayScene.hpp"

BasicShield::BasicShield(float x, float y, Player *player) : MeleeShield("play/shield.png", x, y, player) {
    Size = Engine::Point(PlayScene::BlockSize, PlayScene::BlockSize);
}