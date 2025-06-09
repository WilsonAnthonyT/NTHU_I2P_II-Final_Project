#ifndef EJOJOENEMY_H
#define EJOJOENEMY_H

#include "FlyingEnemy.h"
#include <random>

class EjojoEnemy : public FlyingEnemy {
private:
    // Attack pattern variables
    float timeSinceLastShot;
    float minFireCooldown = 1.0f;
    float maxFireCooldown = 3.0f;
    float nextFireCooldown;

    // Movement pattern variables
    float patternDuration = 4.0f;
    float patternTimer = 0.0f;
    int currentPattern = 0;
    float hoverOffset = 0.0f;
    float fixedAltitude;
    float bulletSpeed;

    // Random engine
    std::mt19937 rng;

    void ShootRandomPattern();
    void UpdateMovementPattern(float deltaTime);
    void SetNextPattern();

public:
    EjojoEnemy(int x, int y);
    void Update(float deltaTime) override;
    void Draw() const override;
};

#endif // EJOJOENEMY_H