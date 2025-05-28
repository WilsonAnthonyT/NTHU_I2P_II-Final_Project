#include "Player.h"

#include <allegro5/allegro_primitives.h>

#include "Engine/GameEngine.hpp"
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"

Player::Player(std::string img, float x, float y, float radius, float speed, float hp): Sprite(img,x,y),speed(speed), hp(hp),flipped(false){
    CollisionRadius = radius;
    MaxHp = hp;
    TintTimer = 0.0f;
    verticalVelocity = 0.0f;
    isJumping = false;
    isFalling = false;
    goDown = false;
    // Calculate jump force for exactly 1.5 blocks (96 pixels) high
    jumpForce = sqrt(2 * PlayScene::Gravity * 96); // 192 pixels/s
}

void Player::Update(float deltaTime) {

    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    Engine::Point inputVelocity(0, 0);

    // Jump only when pressing W and not already jumping
    if (al_key_down(&keyState, ALLEGRO_KEY_W)) {
        if (!isJumping && !isFalling) {  // Can only jump when grounded
            verticalVelocity = -jumpForce;
            isJumping = true;
            isFalling = false;
        }
    }
    if (al_key_down(&keyState, ALLEGRO_KEY_S)) {
        goDown = true;
        goDownTimer = 0.16f; // Reset timer to 0.5 seconds
    }
    else if (goDownTimer > 0) {
        goDownTimer -= deltaTime;
        if (goDownTimer <= 0) {
            goDown = false;
            goDownTimer = 0;
        }
    }
    else {
        goDown = false;
    }
    if (al_key_down(&keyState, ALLEGRO_KEY_A)) inputVelocity.x -= 1;
    if (al_key_down(&keyState, ALLEGRO_KEY_D)) inputVelocity.x += 1;

    // Apply horizontal movement
    if (inputVelocity.x != 0) {
        inputVelocity = inputVelocity.Normalize() * speed;
        float newX = Position.x + inputVelocity.x * deltaTime;

        if (!IsCollision(newX, Position.y)) {
            Position.x = newX;
        } else {
            // Try nudging vertically to slide through gaps
            for (float offset = -5.0f; offset <= 5.0f; offset += 5.0f) {
                if (!IsCollision(newX, Position.y + offset)) {
                    Position.x = newX;
                    Position.y += offset;
                    break;
                }
            }
        }
    }

    // Apply gravity and vertical movement
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (scene) {
        verticalVelocity += PlayScene::Gravity * deltaTime;
    }

    // Update falling state before checking collision
    isFalling = verticalVelocity > 0 && isJumping;

    float newY = Position.y + verticalVelocity * deltaTime;

    // Check vertical collision
    if (!IsCollision(Position.x, newY)) {
        Position.y = newY;
    } else {
        // Hit ground or ceiling
        if (verticalVelocity > 0) {
            // Landed on ground
            isJumping = false;
            isFalling = false;
            verticalVelocity = 0;

            // Find the exact ground position
            float testY = Position.y;
            while (!IsCollision(Position.x, testY + 1)) {
                testY += 1;
            }
            Position.y = testY;
        } else {
            // Hit ceiling
            verticalVelocity = 0;
            isFalling = true; // After hitting ceiling, player will start falling
        }
    }

    // Sprite flipping
    if (inputVelocity.x != 0) {
        bool shouldFlip = inputVelocity.x < 0;
        if (shouldFlip != flipped) {
            flipped = shouldFlip;
            Size.x = shouldFlip ? -fabs(Size.x) : fabs(Size.x);
        }
    }
}
void Player::Draw() const {
    Sprite::Draw();
}

bool Player::IsCollision(float x, float y) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene) return true;

    // Calculate player's square hitbox with 1-pixel tolerance
    const float tolerance = 1.0f;
    float halfSize = PlayScene::BlockSize / 2.0f - tolerance;
    float playerLeft = x - halfSize;
    float playerRight = x + halfSize;
    float playerTop = y - halfSize;
    float playerBottom = y + halfSize;

    // Screen boundaries
    Engine::Point MapBound=PlayScene::GetClientSize();

    if (playerLeft < 0 || playerRight > MapBound.x ||
        playerTop < 0 || playerBottom > MapBound.y) {
        return true;
    }

    // Ground collisions
    int leftTile = static_cast<int>(playerLeft / PlayScene::BlockSize);
    int rightTile = static_cast<int>(playerRight / PlayScene::BlockSize);
    int topTile = static_cast<int>(playerTop / PlayScene::BlockSize);
    int bottomTile = static_cast<int>(playerBottom / PlayScene::BlockSize);

    for (int yTile = topTile; yTile <= bottomTile; yTile++) {
        for (int xTile = leftTile; xTile <= rightTile; xTile++) {
            if (xTile >= 0 && xTile < scene->MapWidth && yTile >= 0 && yTile < scene->MapHeight) {
                if (scene->mapState[yTile][xTile] == PlayScene::TILE_DIRT) {
                    return true;
                }
                // Special handling for platforms
                else if (scene->mapState[yTile][xTile] == PlayScene::TILE_WPLATFORM) {
                    // Only collide if player's bottom is touching the platform's top
                    float platformTop = yTile * PlayScene::BlockSize;
                    float platformBottom = platformTop + PlayScene::BlockSize;

                    // Check if player's bottom is within the top few pixels of the platform
                    const float platformCollisionThreshold = 10.0f; // Adjust this value as needed
                    if (playerBottom >= platformTop &&
                        playerBottom <= platformTop + platformCollisionThreshold &&
                        playerRight > xTile * PlayScene::BlockSize &&
                        playerLeft < (xTile + 1) * PlayScene::BlockSize&&(!isJumping||isFalling)&&!goDown) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

