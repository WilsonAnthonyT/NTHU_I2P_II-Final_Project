#include "EjojoEnemy.h"
#include "Scene/PlayScene.hpp"
#include "Bullet/FireBullet.hpp"
#include <algorithm>
#include <allegro5/allegro_primitives.h>

#include "Engine/GameEngine.hpp"

EjojoEnemy::EjojoEnemy(int x, int y) : FlyingEnemy("play/ejojo.png", x, y, 500, 100.0f, 100, 5, 5, 10),
    rng(std::random_device{}()) {

    Size = Engine::Point(PlayScene::BlockSize*4, PlayScene::BlockSize*2);
    fixedAltitude = PlayScene::BlockSize * 7;
    Position.y = PlayScene::GetClientSize().y - fixedAltitude;
    currentPattern = 0; // Initialize pattern
    speed = 200.0f; // Increased base speed for better visibility

    std::uniform_real_distribution<float> cooldownDist(minFireCooldown, maxFireCooldown);
    nextFireCooldown = cooldownDist(rng);
}

void EjojoEnemy::Update(float deltaTime) {
    timeSinceLastShot += deltaTime;
    patternTimer += deltaTime;

    // Update movement first
    UpdateMovementPattern(deltaTime);

    // Apply movement
    Position.x += VelocityX * deltaTime;
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

void EjojoEnemy::ShootRandomPattern() {
    auto scene = getPlayScene();
    if (!scene || scene->PlayerGroup->GetObjects().empty()) return;

    std::uniform_int_distribution<int> patternDist(0, 3);
    int pattern = patternDist(rng);

    switch(pattern) {
        case 0: { // Single powerful shot
            // scene->BulletGroup->AddNewObject(
            //     new FireBullet(
            //         Engine::Point(Position.x, Position.y + Size.y/2),
            //         Engine::Point(0, 1),
            //         -ALLEGRO_PI/2,
            //         this,
            //         bulletSpeed * 1.5f // Faster bullet
            //     )
            // );
            //AudioHelper::PlayAudio("fire_strong.wav");
            break;
        }

        case 1: { // Spread shot (3 bullets)
            for (int i = -1; i <= 1; i++) {
                // scene->BulletGroup->AddNewObject(
                //     new FireBullet(
                //         Engine::Point(Position.x, Position.y + Size.y/2),
                //         Engine::Point(i * 0.3f, 1), // Angled bullets
                //         -ALLEGRO_PI/2 + (i * 0.3f),
                //         this,
                //         bulletSpeed * 0.8f
                //     )
                // );
            }
            //AudioHelper::PlayAudio("fire_spread.wav");
            break;
        }

        case 2: { // Targeted shot at player position
            auto player = scene->PlayerGroup->GetObjects().front();
            Engine::Point targetDir = (player->Position - Position).Normalize();

            // scene->BulletGroup->AddNewObject(
            //     new FireBullet(
            //         Engine::Point(Position.x, Position.y + Size.y/2),
            //         targetDir,
            //         std::atan2(targetDir.y, targetDir.x),
            //         this,
            //         bulletSpeed
            //     )
            // );
            //AudioHelper::PlayAudio("fire_target.wav");
            break;
        }
    }
}

void EjojoEnemy::UpdateMovementPattern(float deltaTime) {
    if (patternTimer >= patternDuration) {
        SetNextPattern();
        patternTimer = 0.0f;
    }

    switch(currentPattern) {
        case 0: // More pronounced hovering
            hoverOffset = std::sin(patternTimer * 4.0f) * 30.0f; // Increased amplitude
            VelocityX = 0;
            break;

        case 1: // More aggressive zig-zag
            VelocityX = speed * 2.0f * (std::sin(patternTimer * 3.0f) > 0 ? 1 : -1);
            hoverOffset = std::sin(patternTimer * 8.0f) * 15.0f;
            break;

        case 2: // Faster charging
        if (auto scene = getPlayScene(); scene && !scene->PlayerGroup->GetObjects().empty()) {
            auto player = scene->PlayerGroup->GetObjects().front();
            VelocityX = speed * 2.5f * (player->Position.x > Position.x ? 1 : -1);
        }
        break;

        case 3: // Wider circles
        VelocityX = speed * 1.8f * std::cos(patternTimer * 1.2f);
        hoverOffset = std::sin(patternTimer * 1.2f) * 50.0f;
        break;
    }

}
void EjojoEnemy::SetNextPattern() {
    std::uniform_int_distribution<int> patternDist(0, 3);
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
