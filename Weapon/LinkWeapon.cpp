//
// Created by User on 08/06/2025.
//

#include "Player/Player.h"
#include <iostream>
#include <allegro5/allegro_primitives.h>

#include "Enemy/Enemy.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Collider.hpp"

#include "Engine/GameEngine.hpp"
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"

#include "LinkWeapon.h"

LinkWeapon::LinkWeapon(float x, float y, Player *player) : MeleeWeapon("play/lnksword.png",x,y,.4,player, PlayScene::BlockSize*10) {
    Size = Engine::Point(PlayScene::BlockSize * 0.8, PlayScene::BlockSize);
    Anchor = Engine::Point(0.5, 0.8);
}



