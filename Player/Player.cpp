#include "Player.h"

#include <allegro5/allegro_primitives.h>

#include "Engine/GameEngine.hpp"
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"

Player::Player(std::string img, float x, float y, float radius, float speed, float hp): Sprite(img,x,y),speed(speed), hp(hp){
    CollisionRadius = radius;
    MaxHp = hp;
    TintTimer = 0.0f;
}
void Player::Update(float deltaTime) {
    PlayScene* Play = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());

    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    // Handle keyboard input for movement
    float moveSpeed = 200.0f; // Adjust this value to your desired movement speed
    Engine::Point inputVelocity(0, 0);

    if (al_key_down(&keyState, ALLEGRO_KEY_W)) {
        inputVelocity.y -= 1;
    }
    if (al_key_down(&keyState, ALLEGRO_KEY_S)) {
        inputVelocity.y += 1;
    }
    if (al_key_down(&keyState, ALLEGRO_KEY_A)) {
        inputVelocity.x -= 1;
    }
    if (al_key_down(&keyState, ALLEGRO_KEY_D)) {
        inputVelocity.x += 1;
    }

    // Normalize the vector if moving diagonally to maintain consistent speed
    if (inputVelocity.x != 0 || inputVelocity.y != 0) {
        inputVelocity = inputVelocity.Normalize() * speed;
    }

    // Update velocity and position
    Velocity = inputVelocity;
    Position.x += Velocity.x * deltaTime;
    Position.y += Velocity.y * deltaTime;

    // Update rotation based on movement direction if moving
    if (Velocity.x != 0 || Velocity.y != 0) {
        Rotation = atan2(Velocity.y, Velocity.x);
    }
}
void Player::Draw() const {
    Sprite::Draw();
}


