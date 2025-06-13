//
// Created by MiHu on 6/12/2025.
//

#include "SwordSkelly.h"

#include <iostream>

#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DamageText.h"

SwordSkelly::SwordSkelly(int x, int y) : Enemy("play/skeletonsword.png", x, y, 100, PlayScene::BlockSize * 1.25f, 10, 5, 5, 10) {
    currentState = IDLE;
    idleFrameDuration = 0.15f;
    walkFrameDuration = 0.1f;
    attackFrameDuration = 0.08f;
    deathFrameDuration = 0.24f;
    AttackRange = (float)PlayScene::BlockSize/4;
    for (int i = 1; i <= 10; i++) {
        walkAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/skeletonswordwalking-" + std::to_string(i) + ".png"));
    }
    for (int i = 1; i <= 1; i++) {
        idleAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/skeletonswordwalking-" + std::to_string(i) + ".png"));
    }
    for (int i = 1; i <= 4; i++) {
        attackingAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/skeletonswordattack-" + std::to_string(i) + ".png"));
    }
    for (int i = 1; i <= 5; i++) {
        deathAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/skeletonsworddying-" + std::to_string(i) + ".png"));
    }
}

void SwordSkelly::Update(float deltaTime) {
    // Handle death state first
    if (hp <= 0) {
        UpdateAnimation(deltaTime);
        return;
    }

    Enemy::Update(deltaTime);

    // Attack cooldown and triggering
    attackCooldown -= deltaTime;
    if (attackCooldown <= 0 && IsPlayerInRange(Position.x, Position.y, AttackRange)) {
        isAttacking = true;
        attackCooldown = attackInterval;
    }

    UpdateAnimation(deltaTime);
}

void SwordSkelly::UpdateAnimation(float deltaTime) {
    animationTime += deltaTime;

    std::vector<std::shared_ptr<ALLEGRO_BITMAP>>* currentAnimation = nullptr;
    float* currentFrameDuration = nullptr;
    bool looping = true;

    // State priority: Death > Attack > Movement > Idle
    if (hp <= 0) {
        currentState = DEAD;
        currentAnimation = &deathAnimation;
        currentFrameDuration = &deathFrameDuration;
        looping = false;
    }
    else if (isAttacking) {
        currentState = ATTACKING;
        currentAnimation = &attackingAnimation;
        currentFrameDuration = &attackFrameDuration;
        looping = true;  // Attacks shouldn't loop
    }
    else if (isJumping || isFalling || std::abs(VelocityX) > 0.1f) {
        currentState = WALKING;
        currentAnimation = &walkAnimation;
        currentFrameDuration = &walkFrameDuration;
    }
    else {
        currentState = IDLE;
        currentAnimation = &idleAnimation;
        currentFrameDuration = &idleFrameDuration;
    }

    // Update animation frames
    if (currentAnimation && !currentAnimation->empty()) {
        if (animationTime >= *currentFrameDuration) {
            animationTime = 0;

            if (looping) {
                currentFrame = (currentFrame + 1) % currentAnimation->size();
            }
            else if (currentFrame < currentAnimation->size() - 1) {
                currentFrame++;
            }

            // Update bitmap
            if (currentFrame >= 0 && currentFrame < currentAnimation->size()) {
                bmp = (*currentAnimation)[currentFrame];
            } else {
                currentFrame = 0;  // Reset to safe value
                if (!currentAnimation->empty()) {
                    bmp = (*currentAnimation)[0];
                }
            }

            // Handle direction

        }
    }

    // Handle attack completion
    if (currentState == ATTACKING &&
        currentFrame == attackingAnimation.size() - 1)
    {
        isAttacking = false;
    }

    // Handle death completion
    if (currentState == DEAD &&
        currentFrame >= deathAnimation.size() - 1)
    {
        auto *scene = getPlayScene();
        scene->EnemyGroup->RemoveObject(this->GetObjectIterator());
    }
}



