//
// Created by MiHu on 6/12/2025.
//

#ifndef ARCHERSKELLY_H
#define ARCHERSKELLY_H
#include "Enemy.hpp"


class ArcherSkelly : public Enemy {
    bool isAttacking = false;
    float attackCooldown = 0.0f;
    float attackInterval = 1.5f;
    float directionLockTimer = 0.0f;
    const float directionLockDuration = 0.3f;
    float directionChangeCooldown = 0.0f;
public:
    ArcherSkelly(int x, int y);
    void Update(float deltaTime) override;

    void ChasePlayer(const std::vector<Engine::Point> &playerPositions, float deltaTime) override;

    void ShootArrow(const Engine::Point &targetPos, PlayScene *scene);

    void UpdateAnimation(float deltaTime) override;


    // Shooting variables
    float shotCooldown = 0.0f;
    const float shotInterval = 2.0f;
    const float shootingRange = 0.0f; // Will be initialized in constructor

};

#endif //ARCHERSKELLY_H
