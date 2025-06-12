//
// Created by MiHu on 6/12/2025.
//

#ifndef ARCHERSKELLY_H
#define ARCHERSKELLY_H
#include "Enemy.hpp"


class ArcherSkelly : public Enemy {
public:
    ArcherSkelly(int x, int y);
    void Update(float deltaTime) override;

    void ChasePlayer(const std::vector<Engine::Point> &playerPositions, float deltaTime);

    void ShootArrow(const Engine::Point &targetPos, PlayScene *scene);

    // Shooting variables
    float shotCooldown = 0.0f;
    const float shotInterval = 2.0f;
    const float shootingRange = 0.0f; // Will be initialized in constructor

};

#endif //ARCHERSKELLY_H
