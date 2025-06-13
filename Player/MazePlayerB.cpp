//
// Created by MiHu on 6/9/2025.
//

#include "MazePlayerB.h"

#include <allegro5/allegro_primitives.h>

#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"

MazePlayerB::MazePlayerB(float x, float y): Player("play/bryan.png", x, y,PlayScene::BlockSize * 2.25,100) {
    Size = Engine::Point(PlayScene::BlockSize * 0.45, PlayScene::BlockSize * 0.7);
    for (int i = 1; i <= 4; i++) {
        walkAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/bryan-walk" + std::to_string(i) + ".png"));
    }
    for (int i = 1; i <= 2; i++) {
        idleAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/bryan-idle" + std::to_string(i) + ".png"));
    }
    jumpAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/bryan-jump.png"));
}
void MazePlayerB::Update(float deltaTime) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene) return;

    PlayerEnemyCollision(this, deltaTime);

    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    Engine::Point inputVelocity(0, 0);

    if (movementEnabled) {
        // Collect input
        if (al_key_down(&keyState, ALLEGRO_KEY_J)) inputVelocity.x -= 1;
        if (al_key_down(&keyState, ALLEGRO_KEY_L)) inputVelocity.x += 1;
        if (al_key_down(&keyState, ALLEGRO_KEY_I)) inputVelocity.y -= 1;
        if (al_key_down(&keyState, ALLEGRO_KEY_K)) inputVelocity.y += 1;
    }

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

    if (inputVelocity.x != 0 || inputVelocity.y != 0) {
        currentState = WALKING;
    }
    else {
        currentState = IDLE;
    }
    UpdateAnimation(deltaTime);
}

void MazePlayerB::UpdateAnimation(float deltaTime) {
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

// void MazePlayerB::Draw() const {
//     ALLEGRO_BITMAP* target = al_get_target_bitmap(); // Get the current rendering target
//     ALLEGRO_BITMAP* mask = al_create_bitmap(al_get_bitmap_width(target), al_get_bitmap_height(target)); // Create a bitmap for the fog
//
//     // Set the mask as the drawing target
//     al_set_target_bitmap(mask);
//
//     // Fill with black
//     al_clear_to_color(al_map_rgb(0, 0, 0));
//
//     // Draw a transparent circle where the player is
//     al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA); // Set blending mode
//     al_draw_filled_circle(
//         Position.x,
//         Position.y,
//         150.0f, // Radius of visibility
//         al_map_rgba(0, 0, 0, 0) // Fully transparent
//     );
//
//     // Restore the original target
//     al_set_target_bitmap(target);
//
//     // Draw the mask over the screen
//     al_draw_bitmap(mask, 0, 0, 0);
//
//     // Clean up
//     al_destroy_bitmap(mask);
// }
