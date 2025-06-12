#include "RangeWeapon.h"

#include "Player/Player.h"
#include <iostream>
#include <allegro5/allegro_primitives.h>

#include "Bullet/FireBullet.hpp"
#include "Enemy/Enemy.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Collider.hpp"

#include "Engine/GameEngine.hpp"
#include "Engine/Sprite.hpp"
#include "Player/JetpackPlayerA.h"
#include "Player/MeleePlayer.hpp"
#include "Player/RangePlayer.hpp"
#include "Scene/PlayScene.hpp"

RangeWeapon::RangeWeapon(std::string img, float x, float y, float fr, int magazine,Player *player, float speed): Sprite(img,x,y),FireRate(fr), magazine(magazine),flipped(false),player(player), speed(speed){
    Size = Engine::Point(PlayScene::BlockSize * 0.6, PlayScene::BlockSize * 0.6);
    Anchor = Engine::Point(0.5, 0);
    Cooldown = 0;
}

void RangeWeapon::Update(float deltaTime) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    Position.y=player->Position.y+PlayScene::BlockSize*0.05;
    if (player->Visible) this->Visible = true;
    else this->Visible = false;
    flipped=player->flipped;
    if (flipped) {
        Size.x = -fabs(Size.x);
        Position.x=player->Position.x-PlayScene::BlockSize*0.05;
    }else {
        Size.x = fabs(Size.x);
        Position.x=player->Position.x+PlayScene::BlockSize*0.05;
    }
    ALLEGRO_KEYBOARD_STATE keyState;
    al_get_keyboard_state(&keyState);
    MeleePlayer *player1 = dynamic_cast<MeleePlayer *>(player);
    RangePlayer *player2 = dynamic_cast<RangePlayer *>(player);
    JetpackPlayerA *player3 = dynamic_cast<JetpackPlayerA *>(player);

    if (al_key_down(&keyState, ALLEGRO_KEY_C) && Cooldown<=0 && (player2||player3)) {
        AudioHelper::PlayAudio("laser.wav");
        Cooldown = FireRate;
        float bulletdir = abs(Size.x/2) * (flipped? -1.0f : 1.0f);
        scene->BulletGroup->AddNewObject(new FireBullet(Engine::Point(Position.x + bulletdir, Position.y + Size.y * 2 / 5), Engine::Point(flipped?-1:1,0),flipped*(ALLEGRO_PI),this, speed));
    } else {
        Cooldown-=deltaTime;
    }

    if (al_key_down(&keyState, ALLEGRO_KEY_N) && Cooldown<=0 && player1) {
        AudioHelper::PlayAudio("laser.wav");
        Cooldown = FireRate;
        float bulletdir = abs(Size.x/2) * (flipped? -1.0f : 1.0f);
        scene->BulletGroup->AddNewObject(new FireBullet(Engine::Point(Position.x + bulletdir, Position.y + Size.y * 2 / 5), Engine::Point(flipped?-1:1,0),flipped*(ALLEGRO_PI),this, speed));
    } else {
        Cooldown-=deltaTime;
    }
}
void RangeWeapon::Draw() const {
    Sprite::Draw();
}