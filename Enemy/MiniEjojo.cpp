//
// Created by User on 09/06/2025.
//

#include "MiniEjojo.h"
#include "EjojoEnemy.h"
#include "EnemyBullet/EnemyFireBullet.h"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/LightEffect.h"

MiniEjojo::MiniEjojo(int x, int y) : EjojoEnemy("play/EjojoMini.png",x, y) {
    Size = Engine::Point(PlayScene::BlockSize*1, PlayScene::BlockSize*1);
    fixedAltitude = PlayScene::BlockSize * 6;
    Position.y = PlayScene::GetClientSize().y - fixedAltitude;
    currentPattern = 0;
    speed = PlayScene::BlockSize * 1.8f;
    bulletSpeed = PlayScene::BlockSize * 2.5f;

    std::uniform_real_distribution<float> cooldownDist(minFireCooldown, maxFireCooldown);
    nextFireCooldown = cooldownDist(rng);
    getPlayScene()->EffectGroup->AddNewObject(new LightEffect(Position.x, Position.y));
}

void MiniEjojo::Update(float deltaTime) {
    timeSinceLastShot += deltaTime;
    patternTimer += deltaTime;

    // Update movement first
    UpdateMovementPattern(deltaTime);

    // Apply movement
    Position.x += VelocityX * deltaTime * 2.0f;
    Position.y = PlayScene::GetClientSize().y - fixedAltitude + hoverOffset;

    // Shooting logic
    if (timeSinceLastShot >= nextFireCooldown) {
        ShootRandomPattern();
        timeSinceLastShot = 0.0f;
        std::uniform_real_distribution<float> cooldownDist(minFireCooldown, maxFireCooldown);
        nextFireCooldown = cooldownDist(rng);
    }

    // Screen bounds
    Engine::Point screenSize = PlayScene::GetClientSize();
    Position.x = std::max(Size.x/2, std::min(Position.x, screenSize.x - Size.x/2));
}

void MiniEjojo::UpdateMovementPattern(float deltaTime) {
    if (patternTimer >= patternDuration) {
        SetNextPattern();
        patternTimer = 0.0f;
        spawnCooldown = 0.0f; // Reset cooldown when changing patterns
    }

    switch(currentPattern) {
        case 0: // Hovering with limited spawns
            hoverOffset = std::sin(patternTimer * 4.0f) * 30.0f;
            VelocityX = 0;
            break;
        case 1: // Zig-zag movement
            VelocityX = speed * 2.0f * (std::sin(patternTimer * 3.0f) > 0 ? 1 : -1);
            hoverOffset = std::sin(patternTimer * 8.0f) * 15.0f;
            break;

        // case 2: // Charge toward player
        //     if (auto scene = getPlayScene(); scene && !scene->PlayerGroup->GetObjects().empty()) {
        //         auto player = scene->PlayerGroup->GetObjects().front();
        //         VelocityX = speed * 2.5f * (player->Position.x > Position.x ? 1 : -1);
        //     }
        //     break;

        case 2: // Circular movement
            VelocityX = speed * 1.8f * std::cos(patternTimer * 1.2f);
            hoverOffset = std::sin(patternTimer * 1.2f) * 50.0f;
            break;
    }
}

void MiniEjojo::SetNextPattern() {
    std::uniform_int_distribution<int> patternDist(0, 2);
    currentPattern = patternDist(rng);
}

void MiniEjojo::ShootRandomPattern() {
    auto scene = getPlayScene();
    if (!scene || scene->PlayerGroup->GetObjects().empty()) return;

    std::uniform_int_distribution<int> patternDist(0, 2);
    int pattern = patternDist(rng);

    switch(pattern) {
        case 0: {
            float bulletSpacing = 50.0f; // Adjust the spacing between bullets
            int numBullets = 3; // Number of bullets in the rain

            for (int i = 0; i < numBullets; i++) {
                    float bulletX = Position.x - (numBullets / 2) * bulletSpacing + i * bulletSpacing;
                    scene->EnemyBulletGroup->AddNewObject(
                        new EnemyFireBullet(
                            Engine::Point(bulletX, Position.y + Size.y / 2), // Spawn bullets in a line
                            Engine::Point(0, 1), // Move straight down
                            ALLEGRO_PI / 2, // Facing down
                            this,
                            bulletSpeed * 1.0f // Normal speed
                        )
                    );
            }
            break;
        }

        case 1: {
            for (int i = -2; i <= 2; i++) {
                scene->EnemyBulletGroup->AddNewObject(
                    new EnemyFireBullet(
                        Engine::Point(Position.x, Position.y + Size.y/2),
                        Engine::Point(i * 0.3f, 1), // Angled bullets
                        ALLEGRO_PI/2 - (i * 0.15f),
                        this,
                        bulletSpeed * 0.8f
                    )
                );
            }
            break;
        }

    }
}

