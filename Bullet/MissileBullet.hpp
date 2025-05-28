//
// Created by User on 10/05/2025.
//

#ifndef MISSILEBULLET_HPP
#define MISSILEBULLET_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}

class MissileBullet : public Bullet {
    public:
        explicit MissileBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent);
        void OnExplode(Enemy *enemy) override;
};


#endif //MISSILEBULLET_HPP
