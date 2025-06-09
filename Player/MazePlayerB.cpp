//
// Created by MiHu on 6/9/2025.
//

#include "MazePlayerB.h"

#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"

MazePlayerB::MazePlayerB(float x, float y): Player("play/bryan.png", x, y,PlayScene::BlockSize * 2.25,100) {
    Size = Engine::Point(PlayScene::BlockSize * 0.45, PlayScene::BlockSize * 0.7);
}
void MazePlayerB::Update(float deltaTime) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene) return;

    PlayerEnemyCollision(this, deltaTime);

    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    Engine::Point inputVelocity(0, 0);

    // Collect input
    if (al_key_down(&keyState, ALLEGRO_KEY_J)) inputVelocity.x -= 1;
    if (al_key_down(&keyState, ALLEGRO_KEY_L)) inputVelocity.x += 1;
    if (al_key_down(&keyState, ALLEGRO_KEY_I)) inputVelocity.y -= 1;
    if (al_key_down(&keyState, ALLEGRO_KEY_K)) inputVelocity.y += 1;

    // Normalize combined direction to ensure consistent speed
    if (inputVelocity.x != 0 || inputVelocity.y != 0)
        inputVelocity = inputVelocity.Normalize() * speed;

    // Predict new position
    float newX = Position.x + inputVelocity.x * deltaTime;
    float newY = Position.y + inputVelocity.y * deltaTime;

    // Horizontal collision
    if (!IsCollision(newX, Position.y)) {
        Position.x = newX;
    } else {
        // Try nudging vertically
        for (float offset = -5.0f; offset <= 5.0f; offset += 5.0f) {
            if (!IsCollision(newX, Position.y + offset)) {
                Position.x = newX;
                Position.y += offset;
                break;
            }
        }
    }

    // Vertical collision
    if (!IsCollision(Position.x, newY)) {
        Position.y = newY;
    } else {
        // Try nudging horizontally
        for (float offset = -5.0f; offset <= 5.0f; offset += 5.0f) {
            if (!IsCollision(Position.x + offset, newY)) {
                Position.y = newY;
                Position.x += offset;
                break;
            }
        }
    }

    // Flip character sprite
    if (inputVelocity.x != 0) {
        bool shouldFlip = inputVelocity.x < 0;
        if (shouldFlip != flipped) {
            flipped = shouldFlip;
            Size.x = shouldFlip ? -fabs(Size.x) : fabs(Size.x);
        }
    }
}