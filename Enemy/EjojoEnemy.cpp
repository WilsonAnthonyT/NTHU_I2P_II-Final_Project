#include "EjojoEnemy.h"
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

EjojoEnemy::EjojoEnemy(std::string img, int x, int y) : FlyingEnemy(img, x, y, 500, 100.0f, 100, 5, 5, 10),
                                       rng(std::random_device{}()) {
    Size = Engine::Point(PlayScene::BlockSize*4, PlayScene::BlockSize*2);
    fixedAltitude = PlayScene::BlockSize * 8;
    Position.y = PlayScene::GetClientSize().y - fixedAltitude;
    currentPattern = 0;
    speed = PlayScene::BlockSize * 1.8f;
    bulletSpeed = PlayScene::BlockSize * 2.5f;

    //std::uniform_real_distribution<float> cooldownDist(minFireCooldown, maxFireCooldown);
    //nextFireCooldown = cooldownDist(rng);
    nextFireCooldown = 1.0f;
    currentMiniEjojo = 0;

    // Track the initial HP and set the spawn threshold
    initialHP = hp;
    spawnThreshold = initialHP * 0.5f;  // 1/4 loss of HP triggers the spawn
    lastHP = hp;
}

void EjojoEnemy::Update(float deltaTime) {
    if (isFalling) {
        crashProgress = crashShakeTimer > 0 ? 1.0f :
                            std::max(0.5f, 1.0f - ((Position.y) / (getPlayScene()->MapHeight * PlayScene::BlockSize)));

        fallVelocity += PlayScene::Gravity * deltaTime;
        Position.y += fallVelocity * deltaTime;

        smokeTimer -= deltaTime;
        if (smokeTimer <= 0) {
            smokeTimer = 0.05f + (rand() % 100) * 0.001f;
        }


        float groundY = getPlayScene()->MapHeight * PlayScene::BlockSize - Size.y * 1.25f;
        if (Position.y >= groundY) {
            Position.y = groundY;

            // Final crash effects
            if (crashShakeTimer == 0) { // Only trigger once
                crashShakeTimer = crashDelay;

                ExplosionEffect* exp = new ExplosionEffect(Position.x, groundY);
                exp->Size = Engine::Point(Size.y/2, Size.y/2);

                // Big explosion
                getPlayScene()->EffectGroup->AddNewObject(exp);


                // Ground debris
                for (int i = 0; i < 15; i++) {
                    getPlayScene()->GroundEffectGroup->AddNewObject(
                        new DirtyEffect("play/dirty-1.png",
                                       0.5f + (rand() % 100)*0.01f,
                                       Position.x + (rand() % 200 - 100),
                                       groundY - Size.y * 1.25f)
                    );
                }

                // Fire effect
                for (int i = 0; i < 5; i++) {
                    getPlayScene()->EffectGroup->AddNewObject(
                        new LightEffect(Position.x + (rand() % 100 - 50),
                                       groundY - 20)
                    );
                }
            }

            explosionTimer -= deltaTime;
            float posx = Position.x + ((rand() % (int)(Size.x))) - Size.x/2;
            float posy = Position.y  + ((rand() % (int)(Size.y))) - Size.y/2;
            if (explosionTimer <= 0) {
                getPlayScene()->EffectGroup->AddNewObject(
                    new ExplosionEffect(
                        posx, posy
                    )
                );
                explosionTimer = 0.15f + (rand() % 100) * 0.001f;
            }

            // Countdown to removal
            crashShakeTimer -= deltaTime;
            if (crashShakeTimer <= 0 && currentMiniEjojo == 2 && getPlayScene()->TotalMiniEjojo == 0) {
                getPlayScene()->MapId++;
                Engine::GameEngine::GetInstance().ChangeScene("story");
                return;
            }
        }
    }
    else {
        // Original flying behavior
        timeSinceLastShot += deltaTime;
        patternTimer += deltaTime;
        UpdateMovementPattern(deltaTime);

        float newX = Position.x + VelocityX * deltaTime;
        if (!IsCollision(newX, Position.y,false)) {
            Position.x = newX;
        }
        Position.y = PlayScene::GetClientSize().y - fixedAltitude + hoverOffset;

        if (hp <= spawnThreshold) {
            SpawnMiniEjojo();
        }

        if (timeSinceLastShot >= nextFireCooldown) {
            ShootRandomPattern();
            timeSinceLastShot = 0.0f;
            std::uniform_real_distribution<float> cooldownDist(minFireCooldown, maxFireCooldown);
            nextFireCooldown = cooldownDist(rng);
        }
    }

    Sprite::Update(deltaTime);
}

void EjojoEnemy::SpawnMiniEjojo() {
    auto scene = getPlayScene();
    if (!scene) return;

    if (currentMiniEjojo < maxMiniEjojo) {
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
            int numBullets = 10; // Number of bullets in the rain

            for (int i = 0; i < numBullets; i++) {
                    float bulletX = Position.x - (numBullets / 2) * bulletSpacing + i * bulletSpacing;
                    scene->EnemyBulletGroup->AddNewObject(
                        new EnemyFireBullet(
                            Engine::Point(bulletX, Position.y), // Spawn bullets in a line
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
                        Engine::Point(Position.x, Position.y),
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
    if (isFalling) {
        // Darken and redden the ship as it falls


        ALLEGRO_COLOR crashTint = al_map_rgba_f(
            crashProgress * 0.7f,  // R
            crashProgress * 0.5f,  // G
            crashProgress * 0.5f,  // B
            1.0f                  // A
        );

        al_draw_tinted_scaled_bitmap(bmp.get(), crashTint, 0, 0,
            al_get_bitmap_width(bmp.get()), al_get_bitmap_height(bmp.get()),
            Position.x - Size.x/2,Position.y - Size.y/2, Size.x, Size.y, 0);
    }
    else {
        // Normal drawing
        FlyingEnemy::Draw();
    }
    if (Engine::IScene::DebugMode) {
        al_draw_line(Position.x, Position.y,
                    Position.x + VelocityX, Position.y + hoverOffset,
                    al_map_rgb(255, 0, 0), 2.0f);
    }
}

void EjojoEnemy::OnDeath() {
    if (!isFalling) {
        isFalling = true;
        fallVelocity = 0.0f;

        // Disable all normal behaviors
        currentPattern = -1;
        VelocityX = 0;
        hoverOffset = 0;

        // Initial explosion
        ExplosionEffect *exp = new ExplosionEffect(Position.x, Position.y);
        exp->Size = Engine::Point(Size.y, Size.y);
        getPlayScene()->EffectGroup->AddNewObject(exp);

        // Start smoke trail
        smokeTimer = 0.05f;
        explosionTimer = 0.15f;

        // Darken the ship color
        damageTint = al_map_rgb(150, 150, 150);

        // Play crash sound
        //AudioHelper::PlayAudio("spaceship-crash.ogg");
    }
}

// bool EjojoEnemy::IsCollision(float x, float y, bool checkWallsOnly) {
//     PlayScene* scene = getPlayScene();
//     if (!scene) return true;
//
//     Engine::Point mapSize = PlayScene::GetClientSize();
//     float halfSizeX = abs(Size.x / 2);
//     float halfSizeY = abs(Size.y / 2);
//
//     if (x - halfSizeX < 0 || x + halfSizeX > mapSize.x ||
//         y - halfSizeY < 0 || y + halfSizeY > mapSize.y) {
//         return true;
//         }
// }


