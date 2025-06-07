#include "Player/Player.h"
#include <iostream>
#include <allegro5/allegro_primitives.h>

#include "Enemy/Enemy.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Collider.hpp"

#include "Engine/GameEngine.hpp"
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"

#include "MeleeWeapon.h"

MeleeWeapon::MeleeWeapon(std::string img, float x, float y, float Rr,Player *player, float speed): Sprite(img,x,y),RotationRate(Rr), flipped(false),player(player), speed(speed){
    Size = Engine::Point(PlayScene::BlockSize * 0.7, PlayScene::BlockSize * 0.7);
    Anchor = Engine::Point(0, 1);

    isRotating = false;
    cooldown = 0;
}

void MeleeWeapon::Update(float deltaTime){
    Play = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    Position.y=player->Position.y + PlayScene::BlockSize*0.7;
    flipped=player->flipped;
    if (flipped) {
        Size.x = fabs(Size.x);
        Position.x=player->Position.x-PlayScene::BlockSize*0.02;
    }else {
        Size.x = -fabs(Size.x);
        Position.x=player->Position.x+PlayScene::BlockSize*0.02;
    }
    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    if (al_key_down(&keyState, ALLEGRO_KEY_N) && !isRotating && cooldown <= 0) {
        AudioHelper::PlaySample("shovel-sound.mp3");
        isRotating = true;
        rotationProgress = 0.0f;
        cooldown = RotationRate;
    }

    if (isRotating) {
        rotationProgress += deltaTime + deltaTime/speed * speed;

        //Rotate to 180
        if (rotationProgress <= 1.0f) {
            this->Rotation = ALLEGRO_PI/2 * rotationProgress;
        }

        // back to 0
        else if (rotationProgress <= 2.0f) {
            this->Rotation = ALLEGRO_PI/2 * (2.0f - rotationProgress);  // π to 0
        }
        // Animation complete
        else {
            this->Rotation = 0;
            isRotating = false;
        }
    }

    if (cooldown > 0) cooldown -= deltaTime;
    else cooldown = 0;
}

void MeleeWeapon::Draw() const {
    Sprite::Draw();
}


