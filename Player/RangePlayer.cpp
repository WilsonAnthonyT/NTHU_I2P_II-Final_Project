#include "RangePlayer.hpp"

#include <string>

#include "Player.h"
#include "Bullet/FireBullet.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include "Weapon/RangeWeapon.h"

RangePlayer::RangePlayer(float x, float y) : Player("play/arwen.png",x,y, PlayScene::BlockSize * 2.25,100) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (scene->MapId != 6) {
        weapon = new RangeWeapon("play/pistol.png",x,y,.4,30,this, PlayScene::BlockSize*11);
    }
    else {
        weapon = new RangeWeapon("play/guns.png",x,y,.4,30,this, PlayScene::BlockSize*11);
    }
    scene->WeaponGroup->AddNewObject(weapon);
    Size = Engine::Point(PlayScene::BlockSize * 0.4, PlayScene::BlockSize * 0.65);

    for (int i = 1; i <= 4; i++) {
        walkAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/arwen-walk-" + std::to_string(i) + ".png"));
    }
    for (int i = 1; i <= 2; i++) {
        idleAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/arwen-idle-" + std::to_string(i) + ".png"));
    }

    jumpAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/arwen-jump.png"));
}


void RangePlayer::Update(float deltaTime) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene) return;

    PlayerEnemyCollision(this, deltaTime);
    PlayerBulletCollision(this, deltaTime);

    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    Engine::Point inputVelocity(0, 0);

    // Jump only when pressing W and not already jumping
    if (movementEnabled) {
        if (al_key_down(&keyState, ALLEGRO_KEY_W)) {
            if (!isJumping && !isFalling) {  // Can only jump when grounded
                verticalVelocity = -jumpForce;
                isJumping = true;
                isFalling = false;
            }
        }
        if (al_key_down(&keyState, ALLEGRO_KEY_S)) {
            goDown = true;
            goDownTimer = 0.2f; // Reset timer to 0.5 seconds
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
    }

    // Apply gravity and vertical movement
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
            const float tolerance = 1.0f / 64.0f * PlayScene::BlockSize;
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

    if (isJumping || isFalling) {
        currentState = JUMPING;
    }
    else if (inputVelocity.x != 0) {
        currentState = WALKING;
    }
    else {
        currentState = IDLE;
    }
    UpdateAnimation(deltaTime);
}

void RangePlayer::UpdateAnimation(float deltaTime) {
    animationTime += deltaTime;

    std::vector<std::shared_ptr<ALLEGRO_BITMAP>>* currentAnimation = nullptr;

    switch (currentState) {
        case IDLE:
            currentAnimation = &idleAnimation;
            frameDuration = 0.45f;
            break;
        case WALKING:
            currentAnimation = &walkAnimation;
            frameDuration = 0.25f;
            break;
        case JUMPING:
        case FALLING:
            currentAnimation = &jumpAnimation;
            frameDuration = 0.15f;
            break;
    }

    if (animationTime >= frameDuration) {
        animationTime = 0;
        currentFrame = (currentFrame + 1) % currentAnimation->size();
        this->bmp = (*currentAnimation)[currentFrame]; // .get() to access raw pointer

        // Handle sprite flipping
        if (flipped) {
            Size.x = -fabs(Size.x);
        } else {
            Size.x = fabs(Size.x);
        }
    }
}