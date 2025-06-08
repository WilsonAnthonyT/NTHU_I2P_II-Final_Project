//
// Created by User on 08/06/2025.
//

#include "LinkShield.h"

#include "Scene/PlayScene.hpp"

LinkShield::LinkShield(float x, float y, Player *player) : MeleeShield("play/lnkshield.png", x, y, player) {
    Size = Engine::Point(PlayScene::BlockSize, PlayScene::BlockSize);
}
