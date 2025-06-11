//
// Created by User on 09/06/2025.
//

#include "FlyingEnemy.h"

#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include "Bullet/Bullet.hpp"
#include "Enemy.hpp"

#include <map>
#include <unordered_set>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DamageText.h"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/ExplosionEffect.hpp"

FlyingEnemy::FlyingEnemy(std::string img, float x, float y, float radius, float speed, float hp, int money, int scores, float dmg)
    : Enemy(img, x, y, radius, speed, hp, money, scores, dmg) {
    // Flying enemies don't need gravity or jumping
    verticalVelocity = 0;
    isFlying = true;
    // Smaller collision box for flying enemies
    Size = Engine::Point(PlayScene::BlockSize * 0.6, PlayScene::BlockSize * 0.6);
    Anchor = Engine::Point(0.5, 0.5);
}

void FlyingEnemy::Update(float deltaTime) {
    PlayScene* scene = getPlayScene();
    if (!scene || !scene->PlayerGroup || scene->PlayerGroup->GetObjects().empty()) {
        Sprite::Update(deltaTime);
        return;
    }

    // Get player positions
    bool enemyInView = IsInCameraView(Position.x, Position.y);

    // Get player positions and check if any are in view
    std::vector<Engine::Point> playerPositions;
    bool anyPlayerInView = false;

    for (auto& player : scene->PlayerGroup->GetObjects()) {
        if (player->Visible) {
            bool playerInView = IsInCameraView(player->Position.x, player->Position.y);
            anyPlayerInView = anyPlayerInView || playerInView;
            playerPositions.push_back(player->Position);
        }
    }
    // Only chase if both enemy and at least one player are in view
    if (enemyInView && anyPlayerInView) {
        ChasePlayer(playerPositions, deltaTime);
    }

    // Apply movement
    float newX = Position.x + VelocityX * deltaTime;

    if (!IsCollision(newX, Position.y,false)) {
        Position.x = newX;
    }
    Position.y += VelocityY * deltaTime;

    // Keep enemy within screen bounds
    Engine::Point mapSize = PlayScene::GetClientSize();
    Position.x = std::max(Size.x/2, std::min(Position.x, mapSize.x - Size.x/2));
    Position.y = std::max(Size.y/2, std::min(Position.y, mapSize.y - Size.y/2));

    // Knockback effect
    if (isKnockedback) {
        float newX = Position.x + knockbackVelocityX * deltaTime;
        float newY = Position.y + knockbackVelocityY * deltaTime;

        // Only move if not colliding with boundaries
        if (newX > Size.x/2 && newX < mapSize.x - Size.x/2) {
            Position.x = newX;
        }
        if (newY > Size.y/2 && newY < mapSize.y - Size.y/2) {
            Position.y = newY;
        }

        knockbackTimer -= deltaTime;
        Tint = al_map_rgb(255, 0, 0);
        if (knockbackTimer <= 0) {
            isKnockedback = false;
            knockbackVelocityX = 0;
            knockbackVelocityY = 0;
            Tint = al_map_rgb(255, 255, 255);
        }
        Sprite::Update(deltaTime);
        return;
    }

    Sprite::Update(deltaTime);
}

void FlyingEnemy::ChasePlayer(const std::vector<Engine::Point>& playerPositions, float deltaTime) {
    if (playerPositions.empty()) return;

    // Find the closest player
    Engine::Point targetPos = playerPositions[0];
    float minDistance = std::abs(Position.x - targetPos.x) + std::abs(Position.y - targetPos.y);

    for (size_t i = 1; i < playerPositions.size(); ++i) {
        float currentDistance = std::abs(Position.x - playerPositions[i].x) +
                              std::abs(Position.y - playerPositions[i].y);
        if (currentDistance < minDistance) {
            targetPos = playerPositions[i];
            minDistance = currentDistance;
        }
    }

    // Calculate direction vector
    float dx = targetPos.x - Position.x;
    float dy = targetPos.y - Position.y;

    // Normalize the direction vector
    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0) {
        dx /= length;
        dy /= length;
    }

    // Apply movement
    VelocityX = dx * speed;
    VelocityY = dy * speed;

    // Flip sprite based on horizontal direction
    if (VelocityX != 0) {
        Size.x = std::abs(Size.x) * (VelocityX > 0 ? 1 : -1);
    }
}

void FlyingEnemy::Hit(float damage, float PosX, std::string type) {
    hp -= damage;
    ALLEGRO_COLOR damageColor1 = al_map_rgb(255, 165, 0);
    ALLEGRO_COLOR damageColor2 = al_map_rgb(0, 0, 0);
    getPlayScene()->DamageTextGroup->AddNewObject(new DamageText(Position.x+5, Position.y - 10, std::to_string((int)damage), damageColor2));
    getPlayScene()->DamageTextGroup->AddNewObject(new DamageText(Position.x, Position.y - 10, std::to_string((int)damage), damageColor1));

    if (hp <= 0) {
        OnExplode();
        OnDeath();
    }
    else {
        if (type == "melee") {
            float directionX = (Position.x < PosX) ? -1 : 1;
            float directionY = (Position.y < PosX) ? -1 : 1; // Using PosX for Y direction too for simplicity
            knockbackVelocityX = directionX * 500;
            knockbackVelocityY = directionY * 500;
        }
        else if (type == "range") {
            float directionX = (Position.x < PosX) ? -1 : 1;
            float directionY = (Position.y < PosX) ? -1 : 1;
            knockbackVelocityX = directionX * 300;
            knockbackVelocityY = directionY * 300;
        }
        else {
            float directionX = (Position.x < PosX) ? -1 : 1;
            float directionY = (Position.y < PosX) ? -1 : 1;
            knockbackVelocityX = directionX * 300;
            knockbackVelocityY = directionY * 300;
        }
        knockbackTimer = maxKnockbackTime;
        isKnockedback = true;
    }
}

bool FlyingEnemy::IsCollision(float x, float y, bool checkWallsOnly) {
    PlayScene* scene = getPlayScene();
    if (!scene) return true;

    Engine::Point mapSize = PlayScene::GetClientSize();
    float halfSizeX = abs(Size.x / 2);
    float halfSizeY = abs(Size.y / 2);

    if (x - halfSizeX < 0 || x + halfSizeX > mapSize.x ||
        y - halfSizeY < 0 || y + halfSizeY > mapSize.y) {
        return true;
    }

    float left = x - halfSizeX + tolerance;
    float right = x + halfSizeX - tolerance;
    float top = y + tolerance;
    float bottom = y + Size.y - tolerance;

    int tileLeft = static_cast<int>(left / PlayScene::BlockSize);
    int tileRight = static_cast<int>(right / PlayScene::BlockSize);
    int tileTop = static_cast<int>(top / PlayScene::BlockSize);
    int tileBottom = static_cast<int>(bottom / PlayScene::BlockSize);

    for (int yTile = tileTop; yTile <= tileBottom; ++yTile) {
        for (int xTile = tileLeft; xTile <= tileRight; ++xTile) {
            if (xTile >= 0 && xTile < scene->MapWidth &&
                yTile >= 0 && yTile < scene->MapHeight) {
                    int tileType = scene->mapState[yTile][xTile];
                    if (tileType == PlayScene::TILE_DIRT) {
                        return true;
                    }
                }
        }
    }
    return false;
}

void FlyingEnemy::Draw() const {
    Sprite::Draw();

    if (Engine::IScene::DebugMode) {
        // Health bar
        const float healthBarWidth = abs(Size.x*3/4);
        const float healthBarHeight = PlayScene::BlockSize/15;
        const float healthBarOffset = PlayScene::BlockSize/6.4;

        float healthBarX = Position.x - healthBarWidth/2;
        float healthBarY = Position.y - Size.y/2 + healthBarOffset;

        al_draw_filled_rectangle(healthBarX, healthBarY,
                                healthBarX + healthBarWidth, healthBarY + healthBarHeight,
                                al_map_rgb(0, 0, 0));

        float healthRatio = static_cast<float>(hp) / static_cast<float>(MaxHp);
        al_draw_filled_rectangle(healthBarX, healthBarY,
                                healthBarX + healthBarWidth * healthRatio, healthBarY + healthBarHeight,
                                al_map_rgb(255, 0, 0));

        // Hitbox
        float halfSizeX = abs(Size.x / 2);
        float halfSizeY = abs(Size.y / 2);
        float left = Position.x - halfSizeX;
        float right = Position.x + halfSizeX;
        float top = Position.y - halfSizeY;
        float bottom = Position.y + halfSizeY;
        al_draw_rectangle(left, top, right, bottom, al_map_rgb(255, 255, 0), 3);
    }
}