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
    int maxMiniEjojo = 2;

    // Random engine
    std::mt19937 rng;

    // HP tracking variables
    float initialHP;
    float spawnThreshold;
    float lastHP;

    bool isFalling = false;
    float fallVelocity = 0.0f;
    float crashShakeTimer = 0.0f;
    float explosionTimer = 0.0f;
    float smokeTimer = 0.0f;
    float crashDelay = 1.5f; // Time before removal after crash
    ALLEGRO_COLOR damageTint = al_map_rgb(255, 255, 255);
    float crashProgress;

public:
    EjojoEnemy(std::string img, int x, int y);
    void Update(float deltaTime) override;
    void Draw() const override;
    virtual void ShootRandomPattern();
    virtual void UpdateMovementPattern(float deltaTime);
    virtual void SetNextPattern();

    // New method to spawn MiniEjojo
    void SpawnMiniEjojo();
    void OnDeath() override;

    int totalMiniEjojo = 0;

};

#endif // EJOJOENEMY_H
