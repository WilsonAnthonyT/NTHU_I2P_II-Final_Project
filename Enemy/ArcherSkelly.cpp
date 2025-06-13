#include "ArcherSkelly.h"

#include "EnemyBullet/ArrowBullet.h"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"

ArcherSkelly::ArcherSkelly(int x, int y)
    : Enemy("play/skeletonbow.png", x, y, 100, PlayScene::BlockSize * 1.25f, 10, 5, 5, 10),
      shootingRange(PlayScene::BlockSize * 4.5f) {
    getPlayScene()->TotalArcherSkelly++;
    AttackRange = 4.0f * PlayScene::BlockSize;
    currentState = ATTACKING;
    idleFrameDuration = 0.15f;
    walkFrameDuration = 0.1f;
    attackFrameDuration = 0.4f;
    deathFrameDuration = 0.24f;
    attackCooldown = shotCooldown;
    attackInterval = shotInterval;

    // for (int i = 1; i <= 10; i++) {
    //     walkAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/skeletonbowwalking-" + std::to_string(i) + ".png"));
    // }
    // for (int i = 1; i <= 1; i++) {
    //     idleAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/skeletonbowwalking-" + std::to_string(i) + ".png"));
    // }
    for (int i = 1; i <= 4; i++) {
        attackingAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/skeletonbowattack-" + std::to_string(i) + ".png"));
    }
    for (int i = 1; i <= 5; i++) {
        deathAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/skeletonbowdying-" + std::to_string(i) + ".png"));
    }
}

void ArcherSkelly::Update(float deltaTime) {
    PlayScene* scene = getPlayScene();
    if (!scene || !scene->PlayerGroup || scene->PlayerGroup->GetObjects().empty()) {
        Sprite::Update(deltaTime);
        return;
    }

    if (hp <= 0) {
        UpdateAnimation(deltaTime);
        return;
    }


    // Update shot cooldown
    if (shotCooldown > 0) {
        shotCooldown -= deltaTime;
    }

    // Get player positions
    bool enemyInView = IsInCameraView(Position.x, Position.y);
    std::vector<Engine::Point> playerPositions;
    bool anyPlayerInView = false;

    for (auto& player : scene->PlayerGroup->GetObjects()) {
        if (player->Visible) {
            bool playerInView = IsInCameraView(player->Position.x, player->Position.y);
            anyPlayerInView = anyPlayerInView || playerInView;
            playerPositions.push_back(player->Position);
        }
    }

    // Only act if both enemy and at least one player are in view
    if (enemyInView && anyPlayerInView) {
        ChasePlayer(playerPositions, deltaTime);

        // Find closest player for shooting
        if (!playerPositions.empty()) {
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

            // Check if player is in shooting range and cooldown is ready
            float horizontalDistance = std::abs(targetPos.x - Position.x);
            if (horizontalDistance <= shootingRange &&
                shotCooldown <= 0 &&
                !isAttacking) {
                ShootArrow(targetPos, scene);
                shotCooldown = shotInterval;
            }
        }
    }

    // Vertical physics
    verticalVelocity += PlayScene::Gravity * deltaTime;

    // Check if we're on ground
    bool isOnGround = !isJumping && !IsCollision(Position.x, Position.y + 1, true);

    // Horizontal movement and wall detection
    float newX = Position.x + VelocityX * deltaTime;
    bool hitWall = !IsCollision(newX, Position.y, true) && IsCollision(newX, Position.y, false);

    if (!IsCollision(newX, Position.y, false)) {
        Position.x = newX;
    }
    else if (isOnGround && hitWall) {
        // Hit a wall while on ground - jump!
        verticalVelocity = -jumpForce;
        isJumping = true;
        isFalling = false;
    }

    // Vertical movement
    float newY = Position.y + verticalVelocity * deltaTime;
    if (!IsCollision(Position.x, newY, false)) {
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

    // Knockback handling
    if (isKnockedback) {
        float newX = Position.x + knockbackVelocityX * deltaTime;

        if (!IsCollision(newX, Position.y, true)) {
            Position.x = newX;
        } else {
            knockbackVelocityX *= -0.3f;
        }

        knockbackTimer -= deltaTime;
        Tint = al_map_rgb(255, 0, 0);

        if (knockbackTimer <= 0) {
            isKnockedback = false;
            knockbackVelocityX = 0;
            Tint = al_map_rgb(255, 255, 255);
        }
    }


    Sprite::Update(deltaTime);

    UpdateAnimation(deltaTime);
}

void ArcherSkelly::ChasePlayer(const std::vector<Engine::Point>& playerPositions, float deltaTime) {
    if (playerPositions.empty()) return;

    // Find closest player
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

    float horizontalDistance = targetPos.x - Position.x; // Signed distance
    float absoluteHorizontalDistance = std::abs(horizontalDistance);
    float verticalDistance = std::abs(targetPos.y - Position.y);
    const float verticalTolerance = 5.0f;

    // Determine facing direction
    bool shouldFaceRight = horizontalDistance > 0;
    Size.x = std::abs(Size.x) * (shouldFaceRight ? 1 : -1);

    // Calculate desired shooting position (4 blocks away in the correct direction)
    float desiredX = targetPos.x - (shouldFaceRight ? shootingRange : -shootingRange);

    // Only chase if:
    // 1. Not at desired X position (with some tolerance) OR
    // 2. Not at same vertical level
    const float positionTolerance = 5.0f;
    bool atDesiredX = std::abs(Position.x - desiredX) < positionTolerance;
    bool atSameHeight = verticalDistance <= verticalTolerance;

    if (!atDesiredX || !atSameHeight) {
        // If we're on the wrong side of the player, move toward desired position
        Engine::Point moveTarget = targetPos;
        if (!atDesiredX) {
            moveTarget.x = desiredX; // Aim for shooting position rather than player position
        }

        Engine::Point nextMove = BFSPathfind(moveTarget);

        if (nextMove.x != 0 || nextMove.y != 0) {
            float dx = nextMove.x;
            float dy = nextMove.y;

            if (std::abs(dx) > 0.1f) {
                VelocityX = (dx > 0 ? 1 : -1) * speed;
            } else {
                VelocityX = 0;
            }

            if (!isJumping && !isFalling && dy < -0.1f) {
                verticalVelocity = -jumpForce;
                isJumping = true;
            }
        }
    } else {
        // Stop moving when in proper shooting position
        VelocityX = 0;
    }
}

void ArcherSkelly::ShootArrow(const Engine::Point& targetPos, PlayScene* scene) {
    if (isAttacking) return;

    isAttacking = true;
    currentFrame = 0; // Reset animation frame
    animationTime = 0;
    // Calculate direction to player
    Engine::Point direction = targetPos - Position;
    float angle = atan2(direction.y, direction.x);

    // Normalize direction
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction.x /= length;
        direction.y /= length;
    }

    // Adjust spawn position based on facing direction
    float yOffset = PlayScene::BlockSize * 1/3.5;
    float xOffset = (Size.x > 0) ? PlayScene::BlockSize * 0.2f : -PlayScene::BlockSize * 0.2f;

    Engine::Point position = Engine::Point(Position.x + xOffset, Position.y + yOffset);

    scene->EnemyBulletGroup->AddNewObject(
        new ArrowBullet(
            position,
            direction,
            angle,
            this,
            PlayScene::BlockSize * 5.0f
        )
    );
}

void ArcherSkelly::UpdateAnimation(float deltaTime) {
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
    // else if (isJumping || isFalling || std::abs(VelocityX) > 0.1f) {
    //     currentState = WALKING;
    //     currentAnimation = &walkAnimation;
    //     currentFrameDuration = &walkFrameDuration;
    // }
    // else {
    //     currentState = IDLE;
    //     currentAnimation = &idleAnimation;
    //     currentFrameDuration = &idleFrameDuration;
    // }

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
            if (VelocityX != 0) {
                Size.x = std::abs(Size.x) * (VelocityX > 0 ? 1 : -1);
            }
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
        getPlayScene()->TotalArcherSkelly--;
        auto *scene = getPlayScene();
        scene->EnemyGroup->RemoveObject(this->GetObjectIterator());
    }
}

