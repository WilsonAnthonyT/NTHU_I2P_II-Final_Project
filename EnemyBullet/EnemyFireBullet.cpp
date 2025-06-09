//
// Created by User on 09/06/2025.
//

#include "EnemyFireBullet.h"

#include <allegro5/base.h>
#include <random>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"

#include "Player/RangePlayer.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Player;

EnemyFireBullet::EnemyFireBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Engine::Sprite *parent, float speed) :
EnemyBullet("play/bullet-1.png", speed, 2, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
    Size = Engine::Point(PlayScene::BlockSize * 0.8, PlayScene::BlockSize * 0.8);
}


