#include "EjojoEnemy.h"
#include "Scene/PlayScene.hpp"
#include "Bullet/FireBullet.hpp"
#include <algorithm>
#include <allegro5/allegro_primitives.h>

#include "MiniEjojo.h"
#include "EnemyBullet/EnemyFireBullet.h"
#include "EnemyBullet/EnemyBullet.h"
#include "Engine/GameEngine.hpp"
#include "UI/Animation/LightEffect.h"

EjojoEnemy::EjojoEnemy(std::string img, int x, int y) : FlyingEnemy(img, x, y, 500, 100.0f, 100, 5, 5, 10),
                                       rng(std::random_device{}()) {
    Size = Engine::Point(PlayScene::BlockSize*4, PlayScene::BlockSize*2);
    fixedAltitude = PlayScene::BlockSize * 8;
    Position.y = PlayScene::GetClientSize().y - fixedAltitude;
    currentPattern = 0;
    speed = PlayScene::BlockSize * 1.8f;
    bulletSpeed = PlayScene::BlockSize * 2.5f;

    std::uniform_real_distribution<float> cooldownDist(minFireCooldown, maxFireCooldown);
    nextFireCooldown = cooldownDist(rng);
    currentMiniEjojo = 0;

    // Track the initial HP and set the spawn threshold
    initialHP = hp;
    spawnThreshold = initialHP * 0.5f;  // 1/4 loss of HP triggers the spawn
    lastHP = hp;
}

void EjojoEnemy::Update(float deltaTime) {
    auto scene = getPlayScene();
    if (!scene) return;

    timeSinceLastShot += deltaTime;
    patternTimer += deltaTime;

    // Update movement first
    UpdateMovementPattern(deltaTime);

    // Apply movement
    Position.x += VelocityX * deltaTime * 2.0f;
    Position.y = PlayScene::GetClientSize().y - fixedAltitude + hoverOffset;

    // Check if HP has dropped by 1/4
    if (hp == spawnThreshold) {
        SpawnMiniEjojo();
    }

    // Shooting logic
    if (timeSinceLastShot >= nextFireCooldown) {
        ShootRandomPattern();
        timeSinceLastShot = 0.0f;
        std::uniform_real_distribution<float> cooldownDist(minFireCooldown, maxFireCooldown);
        nextFireCooldown = cooldownDist(rng);
    }

    //rain attack pattern

    // Screen bounds
    Engine::Point screenSize = PlayScene::GetClientSize();
    Position.x = std::max(Size.x/2, std::min(Position.x, screenSize.x - Size.x/2));
}

void EjojoEnemy::SpawnMiniEjojo() {
    auto scene = getPlayScene();
    if (!scene) return;

    while (currentMiniEjojo < maxMiniEjojo) {
        float x = Position.x * currentMiniEjojo;
        float y = Position.y - PlayScene::BlockSize * 7;

        auto mini = new MiniEjojo(x, y);
        scene->EnemyGroup->AddNewObject(mini);
        currentMiniEjojo++;
    }
}

void EjojoEnemy::ShootRandomPattern() {
    auto scene = getPlayScene();
    if (!scene) return;

    std::uniform_int_distribution<int> patternDist(0, 2);
    int pattern = patternDist(rng);

    switch(pattern) {
        case 0: {
            float bulletSpacing = 50.0f; // Adjust the spacing between bullets
            int numBullets = 20; // Number of bullets in the rain

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
            for (int i = -10; i <= 10; i++) {
                scene->EnemyBulletGroup->AddNewObject(
                    new EnemyFireBullet(
                        Engine::Point(Position.x, Position.y + Size.y/2),
                        Engine::Point(i * 0.5f, 1), // Angled bullets
                        ALLEGRO_PI/2 - (i * 0.15f),
                        this,
                        bulletSpeed * 0.8f
                    )
                );
            }
            break;
        }

        case 2: {
            static float angleOffset = 0.0f;
            int numBullets = 20; // Number of bullets in the spiral
            float angleStep = ALLEGRO_PI / 10.0f; // Angle between bullets

            for (int i = 0; i < numBullets; i++) {
                float angle = i * angleStep + angleOffset;
                scene->EnemyBulletGroup->AddNewObject(
                    new EnemyFireBullet(
                        Engine::Point(Position.x, Position.y + Size.y / 2),
                        Engine::Point(std::cos(angle), std::sin(angle)), // Bullet direction
                        angle,
                        this,
                        bulletSpeed * 0.8f
                    )
                );
            }

            angleOffset += ALLEGRO_PI / 20.0f; // Gradually rotate the spiral
            break;
        }

    }
}

void EjojoEnemy::UpdateMovementPattern(float deltaTime) {
    if (patternTimer >= patternDuration) {
        SetNextPattern();
        patternTimer = 0.0f;
        spawnCooldown = 0.0f; // Reset cooldown when changing patterns
    }

    switch(currentPattern) {
        case 0: { // Hovering with limited spawns
            hoverOffset = std::sin(patternTimer * 4.0f) * 30.0f;
            VelocityX = 0;
            break;
        }

        case 1: // Zig-zag movement
            VelocityX = speed * 2.0f * (std::sin(patternTimer * 3.0f) > 0 ? 1 : -1);
            hoverOffset = std::sin(patternTimer * 8.0f) * 15.0f;
            break;

        case 2: // Circular movement
            VelocityX = speed * 1.8f * std::cos(patternTimer * 1.2f);
            hoverOffset = std::sin(patternTimer * 1.2f) * 50.0f;
            break;
    }
}

void EjojoEnemy::SetNextPattern() {
    std::uniform_int_distribution<int> patternDist(0, 2);
    currentPattern = patternDist(rng);
}

void EjojoEnemy::Draw() const {
    FlyingEnemy::Draw();
    if (Engine::IScene::DebugMode) {
        al_draw_line(Position.x, Position.y,
                    Position.x + VelocityX, Position.y + hoverOffset,
                    al_map_rgb(255, 0, 0), 2.0f);
    }
}

void EjojoEnemy::OnDeath() {
    maxMiniEjojo += 2;
    SpawnMiniEjojo();
    Enemy::OnDeath();
}

