//
// Created by User on 12/06/2025.
//
#include "Scene/PlayScene.hpp"
#include "Bullet/FireBullet.hpp"
#include <algorithm>
#include <iostream>
#include <allegro5/allegro_primitives.h>

#include "MiniEjojo.h"
#include "EnemyBullet/EnemyFireBullet.h"
#include "EnemyBullet/EnemyBullet.h"
#include "Engine/GameEngine.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/ExplosionEffect.hpp"
#include "UI/Animation/LightEffect.h"

#ifndef EJOJOBOSS_H
#define EJOJOBOSS_H



//
// EjojoBoss.h - Cuphead-style multi-phase boss enemy
//

#ifndef EJOJO_BOSS_H
#define EJOJO_BOSS_H

#include "FlyingEnemy.h"
#include "MiniEjojo.h"
#include <random>

struct AttackPattern {
    std::string name;
    float duration;
    float cooldown;
};

class EjojoBoss : public FlyingEnemy {
private:
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

    float movementBoundsX;
    float movementBoundstopY;
    float movementBoundsbottomY;

    float targetVelocityY = 0;

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

    // Phases
    int currentPhase = 0;
    int maxPhases = 0;
    std::vector<int> phaseTransitionHP;
    float phaseTransitionDuration = 0;
    float phaseTransitionTimer = 0;
    bool isTransitioning = 0;
    bool phaseChangedDuringTransition = false;

    // Movement
    Engine::Point movementBounds;
    Engine::Point targetVelocity;
    float movementSpeed;

    // Attacks
    std::vector<std::vector<AttackPattern>> attackPatterns;
    std::string currentAttack;
    float attackDuration;
    float attackCooldown;

    // Telegraphing
    ALLEGRO_COLOR telegraphColor;
    float telegraphTimer;
    float telegraphDuration;

    // Visual effect

    // Random number generation

    void SetupPhase(int phase);
    void StartPhaseTransition();
    void HandlePhaseTransition(float deltaTime);
    void ChooseNextAttack();
    void PerformAttack();
    void ContinueAttack();
    void UpdateMovement(float deltaTime);
    void UpdateVisualEffects(float deltaTime);
    void HandleCrashSequence(float deltaTime);
    void ExecuteAttack(float deltaTime);

public:
    EjojoBoss(std::string img, int x, int y);
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnDeath() override;
    void UpdateAnimation(float deltaTime) override;
};

#endif // EJOJO_BOSS_H



#endif //EJOJOBOSS_H
