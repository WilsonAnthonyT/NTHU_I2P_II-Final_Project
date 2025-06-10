//
// Created by MiHu on 6/10/2025.
//

#include "Buton.h"
#include "Box.h"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"

PlayScene *Buton::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Buton::Buton(std::string img, float x, float y) : Engine::Sprite(img, x, y){
    active = false;
    Anchor = Engine::Point(0.5,0.5);
    Size.x=PlayScene::BlockSize,Size.y=PlayScene::BlockSize;
    Bitmap = Engine::Resources::GetInstance().GetBitmap(img);
}

void Buton::Update(float deltaTime) {
    if (IsCollision(Position.x, Position.y)) {
        active=true;
        bmp = Engine::Resources::GetInstance().GetBitmap("play/button-active.png");
    }
    if (Engine::IScene::DebugMode && active) {
        this->Tint=al_map_rgb(255,0,0);
    } else if (Engine::IScene::DebugMode && !active) this->Tint=al_map_rgb(255,255,255);
    else if (!Engine::IScene::DebugMode) this->Tint=al_map_rgb(255,255,255);
}

bool Buton::IsCollision(float x, float y) {
    PlayScene* scene = getPlayScene();
    if (!scene) return false;

    // Calculate hitbox based on object size
    float left = x - Size.x/2;
    float right = x + Size.x/2;
    float top = y - Size.y/2;
    float bottom = y + Size.y/2;

    for (auto& it : scene->PlayerGroup->GetObjects()) {
        Player* player = dynamic_cast<Player*>(it);
        // Skip if enemy is not visible, is the healer itself, or invalid
        if (!player || !player->Visible) continue;
        int half_P = abs(player->Size.x) / 2;

        float p_Left = player->Position.x - half_P;
        float p_Right = player->Position.x + half_P;
        float p_Top = player->Position.y;
        float p_Bottom = player->Position.y + player->Size.y;

        bool overlapX = left < p_Right && right > p_Left;
        bool overlapY = top < p_Bottom && bottom > p_Top;

        if (overlapX && overlapY) {
            return true;
        }
    }
    return false;
}