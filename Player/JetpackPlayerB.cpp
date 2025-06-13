//
// Created by User on 13/06/2025.
//

#include "JetpackPlayerB.h"
#include "RangePlayer.hpp"

#include <string>
#include <allegro5/allegro_primitives.h>

#include "Player.h"
#include "Bullet/FireBullet.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"
#include "Weapon/RangeWeapon.h"

JetpackPlayerB::JetpackPlayerB(float x, float y) : Player("play/jetpackB-inactive.png",x,y, PlayScene::BlockSize * 2.25,100) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    weapon = new RangeWeapon("play/guns.png",x,y * 1.5f,.4,30,this, PlayScene::BlockSize*11);
    scene->WeaponGroup->AddNewObject(weapon);
    Size = Engine::Point(PlayScene::BlockSize * 0.55, PlayScene::BlockSize * 0.71);

    // Jetpack-specific properties
    jetpackForce = PlayScene::BlockSize * 3;          // Upward force when jetpack is active
    maxJetpackFuel = INFINITY;          // Maximum fuel in seconds
    jetpackFuel = maxJetpackFuel;   // Current fuel
    fuelRechargeRate = 1.5f;        // Fuel recharge rate per second
    fuelRechargeDelay = 1.0f;       // Delay before recharging starts after last use
    fuelRechargeTimer = 0.0f;       // Timer for recharging delay
    gravityScale = PlayScene::Gravity;            // Normal gravity when not using jetpack
    horizontalSpeed = PlayScene::BlockSize * 2.125f;       // Horizontal movement speed
    fallSpeed = PlayScene::BlockSize * 3;             // Maximum falling speed
}


void JetpackPlayerB::Update(float deltaTime) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene) return;

    PlayerEnemyCollision(this, deltaTime);
    PlayerBulletCollision(this, deltaTime);

    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    Engine::Point inputVelocity(0, 0);

    // Horizontal movement
    if (movementEnabled) {
        if (al_key_down(&keyState, ALLEGRO_KEY_J)) inputVelocity.x -= 1;
        if (al_key_down(&keyState, ALLEGRO_KEY_L)) inputVelocity.x += 1;
        if (al_key_down(&keyState, ALLEGRO_KEY_K)) {
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

        // Apply horizontal movement
        if (inputVelocity.x != 0) {
            inputVelocity = inputVelocity.Normalize() * horizontalSpeed;
            float newX = Position.x + inputVelocity.x * deltaTime;

            if (!IsCollision(newX, Position.y)) {
                Position.x = newX;
            } else {
                // Try nudging vertically to slide through gaps
                for (float offset = -5.0f; offset <= 5.0f; offset += 1.0f) {
                    if (!IsCollision(newX, Position.y + offset)) {
                        Position.x = newX;
                        Position.y += offset;
                        break;
                    }
                }
            }
        }
    }

    // Jetpack control
    bool jetpackActive = false;
    if (movementEnabled && al_key_down(&keyState, ALLEGRO_KEY_I) && jetpackFuel > 0) {
        this->bmp = Engine::Resources::GetInstance().GetBitmap("play/jetpackB-active.png");
        jetpackActive = true;
        verticalVelocity = -jetpackForce; // Apply upward force
        jetpackFuel -= deltaTime; // Consume fuel
        fuelRechargeTimer = fuelRechargeDelay; // Reset recharge timer
    } else {
        this->bmp = Engine::Resources::GetInstance().GetBitmap("play/jetpackB-inactive.png");
        // Apply gravity when jetpack is not active
        verticalVelocity += PlayScene::Gravity * gravityScale * deltaTime;

        // Cap falling speed
        if (verticalVelocity > fallSpeed) {
            verticalVelocity = fallSpeed;
        }

        // Recharge fuel when not using jetpack
        if (jetpackFuel < maxJetpackFuel) {
            if (fuelRechargeTimer > 0) {
                fuelRechargeTimer -= deltaTime;
            } else {
                jetpackFuel += fuelRechargeRate * deltaTime * 2;
                if (jetpackFuel > maxJetpackFuel) {
                    jetpackFuel = maxJetpackFuel;
                }
            }
        }
    }

    // Vertical movement with platform collision handling
    float newY = Position.y + verticalVelocity * deltaTime;

    // Only check for platform collisions when falling (verticalVelocity > 0)
    if (verticalVelocity > 0) {
        if (!IsCollision(Position.x, newY)) {
            Position.y = newY;
        } else {
            // Landed on something (platform or ground)
            verticalVelocity = 0;

            // Find the exact landing position
            float testY = Position.y;
            while (!IsCollision(Position.x, testY + 1)) {
                testY += 1;
            }
            Position.y = testY;
        }
    } else if (verticalVelocity < 0) {
        // Moving upward - check for ceiling collisions
        if (!IsCollision(Position.x, newY)) {
            Position.y = newY;
        } else {
            // Hit ceiling
            verticalVelocity = 0;
            // float testY = Position.y;
            // while (IsCollision(Position.x, testY - 1)) {
            //     testY -= 1;
            // }
            // Position.y = testY;
        }
    } else {
        // No vertical movement
        Position.y = newY;
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

void JetpackPlayerB::Draw() const {
    Player::Draw();
    if (Engine::IScene::DebugMode) {
        const float healthBarWidth = abs(Size.x*3/4);
        const float healthBarHeight = PlayScene::BlockSize/15; // Height of the health bar
        const float healthBarOffset = PlayScene::BlockSize/6.4; // Offset above the enemy

        float healthBarX = Position.x - healthBarWidth/2;
        float healthBarY = Position.y - Size.y/4 + healthBarOffset;
        al_draw_filled_rectangle(healthBarX, healthBarY,
                                healthBarX + healthBarWidth, healthBarY + healthBarHeight,
                                al_map_rgb(0, 0, 0)); // Red background

        float healthRatio = static_cast<float>(jetpackFuel) / static_cast<float>(maxJetpackFuel);
        al_draw_filled_rectangle(healthBarX, healthBarY,
                                healthBarX + healthBarWidth * healthRatio, healthBarY + healthBarHeight,
                                al_map_rgb(180, 180, 255)); // Green health
    }
}
