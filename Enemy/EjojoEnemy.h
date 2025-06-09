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
    float spawnCooldown;

    // Movement pattern variables
    float patternDuration = 4.0f;
    float patternTimer = 0.0f;
    int currentPattern = 0;
    float hoverOffset = 0.0f;
    float fixedAltitude;
    float bulletSpeed;

    // Enemy CHILD
    int currentMiniEjojo = 0;
    const int maxMiniEjojo = 1;

    // Random engine
    std::mt19937 rng;

    // HP tracking variables
    float initialHP;
    float spawnThreshold;
    float lastHP;

public:
    EjojoEnemy(int x, int y);
    void Update(float deltaTime) override;
    void Draw() const override;
    virtual void ShootRandomPattern();
    virtual void UpdateMovementPattern(float deltaTime);
    virtual void SetNextPattern();

    // New method to spawn MiniEjojo
    void SpawnMiniEjojo();

};

#endif // EJOJOENEMY_H
