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
#include "LinkWeapon.h"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Animation/SlashEffect.hpp"

MeleeWeapon::MeleeWeapon(std::string img, float x, float y, float Rr,Player *player, float speed, float damage): Sprite(img,x,y),RotationRate(Rr), flipped(false),player(player), speed(speed), damage(damage){
    isRotating = false;
    cooldown = 0;
}

void MeleeWeapon::Update(float deltaTime){
    Play = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());

    if (player->Visible) this->Visible = true;
    else this->Visible = false;

    Position.y=player->Position.y + PlayScene::BlockSize*0.55;
    flipped=player->flipped;
    if (flipped) {
        Size.x = fabs(Size.x);
        Position.x=player->Position.x+PlayScene::BlockSize*0.18;
    }else {
        Size.x = -fabs(Size.x);
        Position.x=player->Position.x-PlayScene::BlockSize*0.18;
    }
    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);

    if (al_key_down(&keyState, ALLEGRO_KEY_N) && !isRotating && cooldown <= 0) {
        AudioHelper::PlayAudio("slash.mp3");
        isRotating = true;
        rotationProgress = 0.0f;
        cooldown = RotationRate;
    }
    if (isRotating) {
        RotateAnimation(deltaTime);
        CheckHitEnemies(player);
    }

    if (cooldown > 0) cooldown -= deltaTime;
    else cooldown = 0;
}

void MeleeWeapon::Draw() const {
    Sprite::Draw();

    if (Engine::IScene::DebugMode) {
        float halfSize_x = abs(Size.x) / 2;
        float halfSize_y = abs(Size.y) / 2;

        // Define the 4 corners of the bounding box before rotation (centered on Position)
        float left = Position.x - halfSize_x;
        float right = Position.x + halfSize_x;
        float top = Position.y - halfSize_y;
        float bottom = Position.y + halfSize_y;

    }
}


void MeleeWeapon::RotateAnimation(float deltaTime) {
    rotationProgress += deltaTime * speed / PlayScene::BlockSize;
    // Rotate to 180
    if (rotationProgress <= 1.0f) {
        if (flipped) {
            this->Rotation = -(ALLEGRO_PI / 2 * rotationProgress) / (1 - deltaTime);
        } else {
            this->Rotation = ALLEGRO_PI / 2 * rotationProgress / (1 + deltaTime);
        }
    }
    // Back to 0
    else if (rotationProgress <= 2.0f) {
        if (flipped) {
            this->Rotation = -(ALLEGRO_PI / 2 * (2.0f - rotationProgress)) / (1 - deltaTime);
        } else {
            this->Rotation = ALLEGRO_PI / 2 * (2.0f - rotationProgress) / (1 + deltaTime);  // π to 0
        }
    }
    // Animation complete
    else {
        this->Rotation = 0;
        isRotating = false;
        effectPlayed = false;
        PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
        if (scene) {
            for (auto& e : scene->EnemyGroup->GetObjects()) {
                Enemy* enemy = dynamic_cast<Enemy*>(e);
                if (enemy) {
                    enemy->HasBeenHitThisSwing = false;
                }
            }
        }
    }
}

void MeleeWeapon::CheckHitEnemies(Player *player) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    float halfSize_x = abs(player->Size.x) / 2;
    float halfSize_y = abs(player->Size.y) / 2;
    if (!effectPlayed) {
        if (flipped) {
            scene->EffectGroup->AddNewObject(new SlashEffect(player->Position.x - halfSize_x, player->Position.y + halfSize_y, flipped));
        }
        else scene->EffectGroup->AddNewObject(new SlashEffect(player->Position.x + halfSize_x, player->Position.y + halfSize_y, flipped));
        effectPlayed = true;
    }
    if (scene) {
        for (auto& e : scene->EnemyGroup->GetObjects()) {
            Enemy* enemy = dynamic_cast<Enemy*>(e);
            if (enemy && enemy->Visible) {
                float distanceX = player->Position.x - enemy->Position.x;
                float distanceY = player->Position.y - enemy->Position.y;
                float distance = sqrt(distanceX * distanceX + distanceY * distanceY);
                bool isInFront = false;
                if (flipped) {
                    isInFront = enemy->Position.x < player->Position.x;
                } else {
                    isInFront = enemy->Position.x > player->Position.x;
                }

                if (isInFront && distance < PlayScene::BlockSize * 0.9) {
                    // Only apply damage if the enemy hasn't been hit this swing and it's the active phase
                    if (!enemy->HasBeenHitThisSwing && rotationProgress < 1.0f) {
                        std::cout << "Hit enemy!" << std::endl;
                        enemy->Hit(damage, Position.x, "melee");
                        enemy->HasBeenHitThisSwing = true;
                    }
                }
            }
        }
    }
}






