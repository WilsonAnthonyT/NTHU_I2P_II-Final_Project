//
// Created by User on 13/06/2025.
//

#include "JetpackPlayerA.h"
#include "RangePlayer.hpp"

#include <string>

#include "Player.h"
#include "Bullet/FireBullet.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"
#include "Weapon/RangeWeapon.h"

JetpackPlayerA::JetpackPlayerA(float x, float y) : Player("play/arwen.png",x,y, PlayScene::BlockSize * 2.25,100) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    weapon = new RangeWeapon("play/guns.png",x,y,.4,30,this, PlayScene::BlockSize*11);
    scene->WeaponGroup->AddNewObject(weapon);
    Size = Engine::Point(PlayScene::BlockSize * 0.4, PlayScene::BlockSize * 0.65);

    // Jetpack-specific properties
    jetpackForce = 500.0f;          // Upward force when jetpack is active
    maxJetpackFuel = 3.0f;          // Maximum fuel in seconds
    jetpackFuel = maxJetpackFuel;   // Current fuel
    fuelRechargeRate = 0.5f;        // Fuel recharge rate per second
    fuelRechargeDelay = 1.0f;       // Delay before recharging starts after last use
    fuelRechargeTimer = 0.0f;       // Timer for recharging delay
    gravityScale = 1.0f;            // Normal gravity when not using jetpack
    horizontalSpeed = 300.0f;       // Horizontal movement speed
    fallSpeed = 400.0f;             // Maximum falling speed
}


void JetpackPlayerA::Update(float deltaTime) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene) return;

    PlayerEnemyCollision(this, deltaTime);
    PlayerBulletCollision(this, deltaTime);

    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    Engine::Point inputVelocity(0, 0);

    // Horizontal movement
    if (movementEnabled) {
        if (al_key_down(&keyState, ALLEGRO_KEY_A)) inputVelocity.x -= 1;
        if (al_key_down(&keyState, ALLEGRO_KEY_D)) inputVelocity.x += 1;

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
    if (movementEnabled && al_key_down(&keyState, ALLEGRO_KEY_W) && jetpackFuel > 0) {
        jetpackActive = true;
        verticalVelocity = -jetpackForce; // Apply upward force
        jetpackFuel -= deltaTime; // Consume fuel
        fuelRechargeTimer = fuelRechargeDelay; // Reset recharge timer
    } else {
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
                jetpackFuel += fuelRechargeRate * deltaTime;
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
            float testY = Position.y;
            while (IsCollision(Position.x, testY - 1)) {
                testY -= 1;
            }
            Position.y = testY;
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