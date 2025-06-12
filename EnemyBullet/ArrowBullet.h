//
// Created by MiHu on 6/12/2025.
//

#ifndef ARROWBULLET_H
#define ARROWBULLET_H
#include "EnemyBullet.h"


namespace Engine {
    struct Point;
}

class ArrowBullet : public EnemyBullet {
public:
    ArrowBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Engine::Sprite *parent,float speed);
};



#endif //ARROWBULLET_H
