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
#include "UI/Animation/LightEffect.h"

PlayScene *Enemy::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

void Enemy::OnExplode() {
    // getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
    // std::random_device dev;
    // std::mt19937 rng(dev());
    // std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
    // std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
    // for (int i = 0; i < 10; i++) {
    //     // Random add 10 dirty effects.
    //     getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
    // }

    AudioHelper::PlayAudio("coins.mp3");
}
Enemy::Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money, int scores, float dmg) : Engine::Sprite(img, x, y), speed(speed), hp(hp), money(money), scores(scores), damage(dmg) {
    CollisionRadius = Size.x/2;
    reachEndTime = 0;
    boostedsoundplayed = false;
    MaxHp = hp;
    TintTimer = 0.0f;
    verticalVelocity = 0.0f;
    isJumping = false;
    isFalling = false;
    goDown = false;
    // Calculate jump force for exactly 1.5 blocks (96 pixels) high
    jumpForce = sqrt(2 * PlayScene::Gravity * ((PlayScene::BlockSize)/2)*3); // 192 pixels/s
    Size = Engine::Point(PlayScene::BlockSize * 0.7, PlayScene::BlockSize * 0.7);
    Anchor = Engine::Point(0.5, 0);
    tolerance = 1.0 / 64.0 * PlayScene::BlockSize;
    VelocityX = speed;
    startChase = false;
}

//trying
void Enemy::OnDeath() {
    auto *scene = getPlayScene();
    scene->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
    scene->EnemyGroup->RemoveObject(GetObjectIterator());
}

void Enemy::Hit(float damage, float PosX, std::string type) {
    hp -= damage;
    ALLEGRO_COLOR damageColor1 = al_map_rgb(255, 165, 0);
    ALLEGRO_COLOR damageColor2 = al_map_rgb(0, 0, 0);
    getPlayScene()->DamageTextGroup->AddNewObject(new DamageText(Position.x+5, Position.y - 10, std::to_string((int)damage), damageColor2));
    getPlayScene()->DamageTextGroup->AddNewObject(new DamageText(Position.x, Position.y - 10, std::to_string((int)damage), damageColor1));

    if (hp <= 0) {
        hp = 0;
        OnExplode();
        OnDeath();
    }
    else {
        if (type == "melee") {
            float direction = (abs(Position.x) < abs(PosX)) ? -1 : 1;
            knockbackVelocityX = direction * PlayScene::BlockSize * 7.0;
        }
        else if (type == "range") {
            float direction = (abs(Position.x) < abs(PosX)) ? -1 : 1;
            knockbackVelocityX = direction * PlayScene::BlockSize * 3.0;
        }
        else {
            float direction = (abs(Position.x) < abs(PosX)) ? -1 : 1;
            knockbackVelocityX = direction * PlayScene::BlockSize * 4.5;
        }
        knockbackTimer = maxKnockbackTime;
        isKnockedback = true;
    }
}

void Enemy::ChasePlayer(const std::vector<Engine::Point>& playerPositions, float deltaTime) {
    if (playerPositions.empty()) return;

    // Find the closest player using Manhattan distance
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

    // Calculate direction using BFS pathfinding
    Engine::Point nextMove = BFSPathfind(targetPos);

    // Apply movement based on BFS result
    if (nextMove.x != 0 || nextMove.y != 0) {
        float dx = nextMove.x;
        float dy = nextMove.y;

        // Horizontal movement
        if (std::abs(dx) > 0.1f) {
            VelocityX = (dx > 0 ? 1 : -1) * speed;
            // Flip sprite based on direction
            Size.x = std::abs(Size.x) * (VelocityX > 0 ? 1 : -1);
        } else {
            VelocityX = 0;
        }

        // Vertical movement (jumping)
        if (!isJumping && !isFalling && dy < -0.1f) {
            verticalVelocity = -jumpForce;
            isJumping = true;
        }
    }
}

Engine::Point Enemy::BFSPathfind(const Engine::Point& target) {
    // Convert positions to tile coordinates
    int startX = static_cast<int>(Position.x / PlayScene::BlockSize);
    int startY = static_cast<int>(Position.y / PlayScene::BlockSize);
    int targetX = static_cast<int>(target.x / PlayScene::BlockSize);
    int targetY = static_cast<int>(target.y / PlayScene::BlockSize);

    // If already at target tile, return zero movement
    if (startX == targetX && startY == targetY) {
        return Engine::Point(0, 0);
    }

    // BFS setup
    std::queue<std::pair<int, int>> queue;
    std::unordered_set<int> visited;
    std::map<std::pair<int, int>, std::pair<int, int>> parent;

    queue.push({startX, startY});
    visited.insert(startY * getPlayScene()->MapWidth + startX);

    // Possible movement directions (left, right, up, down)
    const int dx[] = {-1, 1, 0, 0};
    const int dy[] = {0, 0, -1, 1};

    bool found = false;

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();

        // Check if we've reached the target
        if (current.first == targetX && current.second == targetY) {
            found = true;
            break;
        }

        // Explore neighbors
        for (int i = 0; i < 4; i++) {
            int nx = current.first + dx[i];
            int ny = current.second + dy[i];

            // Check bounds and walkability
            if (nx >= 0 && nx < getPlayScene()->MapWidth &&
                ny >= 0 && ny < getPlayScene()->MapHeight) {

                int key = ny * getPlayScene()->MapWidth + nx;

                // Consider platform tiles as walkable
                bool isWalkable = getPlayScene()->mapState[ny][nx] == PlayScene::TILE_WPLATFORM ||
                                  getPlayScene()->mapState[ny][nx] == PlayScene::TILE_DIRT;

                if (visited.find(key) == visited.end() && isWalkable) {
                    visited.insert(key);
                    parent[{nx, ny}] = current;
                    queue.push({nx, ny});
                }
            }
        }
    }

    // Reconstruct path if found
    if (found) {
        std::vector<std::pair<int, int>> path;
        auto current = std::make_pair(targetX, targetY);

        while (current != std::make_pair(startX, startY)) {
            path.push_back(current);
            current = parent[current];
        }

        // Return the first step direction (in world coordinates)
        if (!path.empty()) {
            auto nextStep = path.back();
            return Engine::Point(
                (nextStep.first - startX) * PlayScene::BlockSize,
                (nextStep.second - startY) * PlayScene::BlockSize
            );
        }
    }

    // Fallback to simple movement if no path found
    return Engine::Point(
        target.x - Position.x,
        target.y - Position.y
    );
}

void Enemy::Update(float deltaTime) {
    PlayScene* scene = getPlayScene();
    if (!scene || !scene->PlayerGroup || scene->PlayerGroup->GetObjects().empty()) {
        Sprite::Update(deltaTime);
        return;
    }

    // Get player positions
    bool enemyInView = IsInCameraView(Position.x, Position.y);

    // Get player positions and check if any are in view
    std::vector<Engine::Point> playerPositions;

    for (auto& player : scene->PlayerGroup->GetObjects()) {
        if (player->Visible) {
            playerPositions.push_back(player->Position);
        }
    }

    // Only chase if both enemy and at least one player are in view
    if (enemyInView) startChase = true;

    if (startChase) ChasePlayer(playerPositions, deltaTime);
    else {
        playerPositions.clear();
        ChasePlayer(playerPositions, deltaTime);
    }

    // Vertical physics
    verticalVelocity += PlayScene::Gravity * deltaTime;

    // Check if we're on ground (using a special wall-only check)
    bool isOnGround = !isJumping && !IsCollision(Position.x, Position.y + 1, true);

    // Horizontal movement and wall detection
    float newX = Position.x;
    if (startChase)  newX += VelocityX * deltaTime;
    bool hitWall = !IsCollision(newX, Position.y, true) && IsCollision(newX, Position.y,false);

    if (!IsCollision(newX, Position.y,false)) {
        Position.x = newX;
    }
    else if (isOnGround && hitWall) {
        // Hit a wall while on ground - jump!
        verticalVelocity = -jumpForce;
        isJumping = true;
        isFalling = false;
    }

    // Vertical movement (using normal collision check)
    float newY = Position.y + verticalVelocity * deltaTime;
    if (!IsCollision(Position.x, newY,false)) {
        Position.y = newY;
    } else {
        if (verticalVelocity > 0) {
            // Hit ground
            isFalling = false;
            isJumping = false;
        } else {
            // Hit ceiling
            isJumping = false;
            isFalling = true;
        }
        verticalVelocity = 0;
    }

    //knockback
    if (isKnockedback) {
        float newX = Position.x + knockbackVelocityX * deltaTime;

        // Only move if not colliding with wall
        if (!IsCollision(newX, Position.y, true)) {
            Position.x = newX;
        } else {
            // If hitting wall, bounce back slightly
            knockbackVelocityX *= -0.3f;
        }
        knockbackTimer -= deltaTime;
        Tint = al_map_rgb(255, 0, 0);
        if (knockbackTimer <= 0) {
            isKnockedback = false;
            knockbackVelocityX = 0;
            Tint = al_map_rgb(255, 255, 255);
        }
        Sprite::Update(deltaTime);
        return; // Skip normal chasing while being knocked back
    }

    Sprite::Update(deltaTime);
}


void Enemy::Draw() const {
    Sprite::Draw();

    //knockback
    if (Engine::IScene::DebugMode) {
        //health bar
        const float healthBarWidth = abs(Size.x*3/4);
        const float healthBarHeight = PlayScene::BlockSize/15; // Height of the health bar
        const float healthBarOffset = PlayScene::BlockSize/6.4; // Offset above the enemy

        float healthBarX = Position.x - healthBarWidth/2;
        float healthBarY = Position.y - Size.y/2 + healthBarOffset;

        al_draw_filled_rectangle(healthBarX, healthBarY,
                                healthBarX + healthBarWidth, healthBarY + healthBarHeight,
                                al_map_rgb(0, 0, 0)); // Red background

        float healthRatio = static_cast<float>(hp) / static_cast<float>(MaxHp);
        al_draw_filled_rectangle(healthBarX, healthBarY,
                                healthBarX + healthBarWidth * healthRatio, healthBarY + healthBarHeight,
                                al_map_rgb(255, 0, 0)); // Green health

        //HITBOX

        float halfSizeX = abs(Size.x / 2);
        float left = Position.x - halfSizeX;
        float right = Position.x + halfSizeX ;
        float top = Position.y;
        float bottom = Position.y + abs(Size.y);
        al_draw_rectangle(left, top, right, bottom, al_map_rgb(255,255,0), 3);
    }
}

bool Enemy::IsCollision(float x, float y, bool checkWallsOnly) {
    PlayScene* scene = getPlayScene();
    if (!scene) return true;

    float halfSizeX = abs(Size.x / 2);
    float left = x - halfSizeX + tolerance;
    float right = x + halfSizeX - tolerance;
    float top = y + tolerance;
    float bottom = y + Size.y - tolerance;

    // Check out-of-bounds
    Engine::Point mapSize = PlayScene::GetClientSize();
    if (left < 0 || right > mapSize.x || top < 0 || bottom > mapSize.y)
        return true;

    // Check collision with map tiles
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
                    // Always collide with dirt
                    return true;
                }

                if (tileType == PlayScene::TILE_WPLATFORM && !checkWallsOnly) {
                    float tileTopY = yTile * PlayScene::BlockSize;
                    const float threshold = PlayScene::BlockSize / 8.0f;
                    // Only collide if falling and hitting from above
                    if (verticalVelocity > 0 && bottom >= tileTopY && bottom <= tileTopY + threshold) {
                        return true;
                    }
                }
                }
        }
    }
    return false;
}

//in camera view
bool Enemy::IsInCameraView(float x, float y) {
    PlayScene* scene = getPlayScene();
    if (!scene) return false;

    // Get camera position and screen dimensions
    float cameraX = scene->Camera.x;
    float cameraY = scene->Camera.y;
    float screenW = Engine::GameEngine::GetInstance().GetScreenWidth();
    float screenH = Engine::GameEngine::GetInstance().GetScreenHeight();

    // Check if point is within camera view with some padding// Optional padding around screen edges
    return (x >= cameraX &&
            x <= cameraX + screenW &&
            y >= cameraY &&
            y <= cameraY + screenH);
}


//return HP (for missile)
float Enemy::getHp() const {
    return hp;
}

//for booster
float Enemy::getSpeed() const {
    return speed;
}

void Enemy::SetSpeed(float speed) {
    this->speed = speed;
}





