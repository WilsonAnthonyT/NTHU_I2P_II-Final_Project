#ifndef FIREBULLET_HPP
#define FIREBULLET_HPP
#include "Bullet.hpp"
#include "Weapon/RangeWeapon.h"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class FireBullet : public Bullet {
public:
    explicit FireBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, RangeWeapon *parent,float speed);
    void OnExplode(Enemy *enemy) override;
};
#endif   // FIREBULLET_HPP
