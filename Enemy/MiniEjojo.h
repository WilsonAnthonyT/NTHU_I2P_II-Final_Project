//
// Created by User on 09/06/2025.
//

#ifndef MINIEJOJO_H
#define MINIEJOJO_H
#include "EjojoEnemy.h"
#include "Engine/GameEngine.hpp"


class MiniEjojo : public EjojoEnemy {
    float timeSinceLastShot;
    float minFireCooldown = 1.0f;
    float maxFireCooldown = 3.0f;
    float nextFireCooldown;
    float spawnCooldown;

    // Movement pattern variables
    float patternDuration = 4.0f;
    float patternTimer = 0.0f;
    int currentPattern = 0;
    float hoverOffset = 0.0f;
    float fixedAltitude;
    float bulletSpeed;

    std::mt19937 rng;
public:
    MiniEjojo(int x, int y);
    void UpdateMovementPattern(float deltaTime) override;
    void SetNextPattern() override;
    void ShootRandomPattern() override;
    void Update(float deltaTime) override;
};



#endif //MINIEJOJO_H
