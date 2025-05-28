//
// Created by User on 10/05/2025.
//
#include <allegro5/base.h>
#include <random>
#include <string>
#include <cmath>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Engine/Collider.hpp"
#include "MissileBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/ExplosionEffect.hpp"

class Turret;
MissileBullet::MissileBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Turret *parent) :
Bullet("play/bullet-4.png", 900, 5, position, forwardDirection, rotation + ALLEGRO_PI / 2, parent){

}
void MissileBullet::OnExplode(Enemy *enemy) {
    if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enemy->Position, enemy->CollisionRadius)
        && (enemy->getHp() - damage) > 0) {
        getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
    }

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-3.png", dist(rng), enemy->Position.x, enemy->Position.y));

    //for AOE
    float splashRadius = 50.0f;
    PlayScene *scene = getPlayScene();

    for (auto& e : scene->EnemyGroup->GetObjects()) {
        Enemy* EnemyinArea = dynamic_cast<Enemy*>(e);
        if (EnemyinArea && (EnemyinArea != enemy)) {
            float a = std::abs(EnemyinArea->Position.x - enemy->Position.x);
            float b = std::abs(EnemyinArea->Position.y - enemy->Position.y);
            /*trying distance formula*/
            if (sqrt(a * a + b * b) <= splashRadius) {
                EnemyinArea->Hit(damage);
            }
        }
    }
}

