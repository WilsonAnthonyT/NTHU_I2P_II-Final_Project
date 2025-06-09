//
// Created by User on 09/06/2025.
//
#include "EnemyBullet.h"
#include "Enemy/Enemy.hpp"

#ifndef ENEMYFIREBULLET_H
#define ENEMYFIREBULLET_H

namespace Engine {
    struct Point;
}

class EnemyFireBullet : public EnemyBullet {
    public:
    EnemyFireBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Engine::Sprite *parent,float speed);
};



#endif //ENEMYFIREBULLET_H
