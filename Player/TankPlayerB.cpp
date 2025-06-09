//
// Created by User on 09/06/2025.
//

#include "TankPlayerB.h"

#include "Bullet/FireBullet.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"

TankPlayerB::TankPlayerB(float x, float y): Player("play/tankplayerb.png", x, y,PlayScene::BlockSize * 2.25,100) {
    Size = Engine::Point(PlayScene::BlockSize, PlayScene::BlockSize);
    FireRate = 0.4f;
    Cooldown = 0.0f;
    bulletspeed= PlayScene::BlockSize*11;
}
void TankPlayerB::Update(float deltaTime) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene) return;

    Hit(this, deltaTime);

    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    Engine::Point inputVelocity(0, 0);

    if (al_key_down(&keyState, ALLEGRO_KEY_J)) inputVelocity.x -= 1;
    if (al_key_down(&keyState, ALLEGRO_KEY_L)) inputVelocity.x += 1;

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
    if (al_key_down(&keyState, ALLEGRO_KEY_N) && Cooldown<=0) {
        AudioHelper::PlayAudio("laser.wav");
        Cooldown = FireRate;
        scene->BulletGroup->AddNewObject(new FireBullet(Engine::Point(Position.x, Position.y), Engine::Point(0,-1),-ALLEGRO_PI/2,this, bulletspeed));
    } else {
        Cooldown-=deltaTime;
    }
}
