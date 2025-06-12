#ifndef EJOJOENEMY_H
#define EJOJOENEMY_H

#include "FlyingEnemy.h"
#include <random>

class EjojoEnemy : public FlyingEnemy {
private:
    // Attack pattern variables

    float timeSinceLastShot = 0.0f;
    float minFireCooldown = 1.0f;
    float maxFireCooldown = 3.0f;
    float nextFireCooldown = 0.0f;
    float spawnCooldown = 0.0f;

    // Movement pattern variables
    float patternDuration = 2.0f;
    float patternTimer = 0.0f;
    int currentPattern = 0;
    float hoverOffset = 0.0f;
    float fixedAltitude = 0.0f;
    float bulletSpeed = 0.0f;

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

    //bool IsCollision(float x, float y, bool checkWallsOnly) override;
    int totalMiniEjojo = 0;

};

#endif // EJOJOENEMY_H
