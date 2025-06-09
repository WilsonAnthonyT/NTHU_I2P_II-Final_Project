//
// Created by MiHu on 6/9/2025.
//

#include "Sensor.h"

#include "Box.h"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"

PlayScene *Sensor::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Sensor::Sensor(std::string img, float x, float y, int weight) : Engine::Sprite(img, x, y){
    Weight = weight;
    active = false;
    Anchor = Engine::Point(0.5, -1);
    Size.x=PlayScene::BlockSize,Size.y=PlayScene::BlockSize;
}

void Sensor::Update(float deltaTime) {
    if (IsCollision(Position.x, Position.y)>=Weight) {
        active=true;
        bmp = Engine::Resources::GetInstance().GetBitmap("play/sensor-active.png");
    }else active=false,bmp = Engine::Resources::GetInstance().GetBitmap("play/sensor.png");;

    if (Engine::IScene::DebugMode && active) {
        this->Tint=al_map_rgb(255,0,0);
    } else if (Engine::IScene::DebugMode && !active) this->Tint=al_map_rgb(255,255,255);
    else if (!Engine::IScene::DebugMode) this->Tint=al_map_rgb(255,255,255);
}

void Sensor::Draw() const{
    Sprite::Draw();
}
int Sensor::IsCollision(float x, float y) {
    int count = 0;
    PlayScene* scene = getPlayScene();
    if (!scene) return 0;

    // Calculate hitbox based on object size
    float left = x - Size.x/2;
    float right = x + Size.x/2;
    float top = y;
    float bottom = y + Size.y;

    for (auto& it : scene->PlayerGroup->GetObjects()) {
        Player* player = dynamic_cast<Player*>(it);
        // Skip if enemy is not visible, is the healer itself, or invalid
        if (!player || !player->Visible) continue;
        int half_P = abs(player->Size.x) / 2;

        float p_Left = player->Position.x - half_P;
        float p_Right = player->Position.x + half_P;
        float p_Top = player->Position.y;
        float p_Bottom = player->Position.y + player->Size.y;

        bool overlapX = left + PlayScene::BlockSize/15 < p_Right && right - PlayScene::BlockSize/15 > p_Left;
        bool overlapY = top < p_Bottom && bottom > p_Top;

        if (overlapX && overlapY) {
            count++;
        }
    }

    for (auto& it : scene->InteractiveBlockGroup->GetObjects()) {
        Box* friends = dynamic_cast<Box*>(it);
        // Skip if enemy is not visible, is the healer itself, or invalid
        if (!friends || !friends->Visible) continue;
        int half_P2 = abs(friends->Size.x) / 2;

        float p2_Left = friends->Position.x - half_P2;
        float p2_Right = friends->Position.x + half_P2;
        float p2_Top = friends->Position.y;
        float p2_Bottom = friends->Position.y + friends->Size.y;

        bool overlapX = left < p2_Right && right > p2_Left;
        bool overlapY = top < p2_Bottom && bottom > p2_Top;

        if (overlapX && overlapY) {
            count++;
        }
    }

    return count;
}