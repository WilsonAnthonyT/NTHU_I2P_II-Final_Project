//
// Created by MiHu on 6/14/2025.
//

#ifndef REDBULLET_H
#define REDBULLET_H
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}

class RedBullet : public Bullet {
public:
    explicit RedBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Sprite *parent,float speed);
    void OnExplode(Enemy *enemy) override;
};



#endif //REDBULLET_H
