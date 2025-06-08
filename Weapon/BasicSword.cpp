//
// Created by User on 08/06/2025.
//

#include "BasicSword.h"

BasicSword::BasicSword(float x, float y, Player *player) : MeleeWeapon("play/sword.png",x,y,0.75,player, PlayScene::BlockSize*10, 3.0) {
    Size = Engine::Point(PlayScene::BlockSize * 0.8, PlayScene::BlockSize);
    Anchor = Engine::Point(0.5, 0.8);
}