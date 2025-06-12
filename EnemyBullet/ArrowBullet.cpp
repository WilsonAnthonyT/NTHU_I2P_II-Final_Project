//
// Created by MiHu on 6/12/2025.
//

#include "ArrowBullet.h"

#include "Scene/PlayScene.hpp"

ArrowBullet::ArrowBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Engine::Sprite *parent, float speed) :
EnemyBullet("play/arrow.png", speed, 2, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
    Size = Engine::Point(PlayScene::BlockSize * 0.5, PlayScene::BlockSize * 0.5);
}
