//
// Created by MiHu on 6/14/2025.
//

#ifndef ENEMYREDBULLET_H
#define ENEMYREDBULLET_H


#include "EnemyBullet.h"
#include "Enemy/Enemy.hpp"

namespace Engine {
    struct Point;
}

class EnemyRedBullet : public EnemyBullet {
public:
    EnemyRedBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Engine::Sprite *parent,float speed);
};





#endif //ENEMYREDBULLET_H
