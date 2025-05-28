//
// Created by User on 12/05/2025.
//
#include <string>
#include "Enemy.hpp"
#include "Scene/PlayScene.hpp"
#include "BoosterEnemy.hpp"
#include "UI/Animation/ExplosionEffect.hpp"
#include "UI/Animation/LightEffect.hpp"

#include <allegro5/allegro_primitives.h>

BoosterEnemy::BoosterEnemy(int x, int y) : Enemy("play/enemy-4.png", x, y, 25, 15, 80, 40, 50, false) {
}

void BoosterEnemy::applyBooster(Enemy *enemy) {
    if (PlayScene::DebugMode) {
        al_draw_circle(Position.x, Position.y, boostRadius, al_map_rgb(255, 0, 0), 2);
    }
    PlayScene *scene = getPlayScene();
    for (auto& e : scene->EnemyGroup->GetObjects()) {
        Enemy* otherEnemy = dynamic_cast<Enemy*>(e);
        if (otherEnemy && (otherEnemy != enemy) && !otherEnemy->isBoosted()) {
            float a = std::abs(otherEnemy->Position.x - enemy->Position.x);
            float b = std::abs(otherEnemy->Position.y - enemy->Position.y);
            if (sqrt(a * a + b * b) <= boostRadius) {
                otherEnemy->Tint = al_map_rgb(255, 0, 0);
                otherEnemy->SetSpeed(otherEnemy->getSpeed() * 2.75);
                scene->EffectGroup->AddNewObject(new LightEffect(Position.x, Position.y));
                otherEnemy->setBoosted(true);
            }
        }
    }
}

void BoosterEnemy::Draw() const{
    Enemy::Draw();
    if (PlayScene::DebugMode) {
        al_draw_circle(Position.x, Position.y, boostRadius, al_map_rgb(255, 0, 0), 2);
    }
}

