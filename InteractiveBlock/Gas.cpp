//
// Created by MiHu on 6/13/2025.
//

#include "Gas.h"
#include "Engine/GameEngine.hpp"

PlayScene *Gas::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Gas::Gas(std::string img, float x, float y) :Sprite(img, x, y){
    Anchor = Engine::Point(0.5, 0);
    Size.x=3*PlayScene::BlockSize,Size.y=PlayScene::BlockSize;
}

void Gas::Update(float deltaTime) {
    IsCollision(Position.x, Position.y);
}

void Gas::IsCollision(float x, float y) {
    PlayScene* scene = getPlayScene();
    if (!scene) return;

    // Calculate hitbox based on object size
    float left = x - Size.x/2;
    float right = x + Size.x/2;
    float top = y;
    float bottom = y + Size.y;

    for (auto& it : scene->PlayerGroup->GetObjects()) {
        Player* player = dynamic_cast<Player*>(it);
        if (!player || !player->Visible) continue;
        int half_P = abs(player->Size.x) / 2;

        float p_Left = player->Position.x - half_P;
        float p_Right = player->Position.x + half_P;
        float p_Top = player->Position.y;
        float p_Bottom = player->Position.y + player->Size.y;

        bool overlapX = left < p_Right && right > p_Left;
        bool overlapY = top < p_Bottom && bottom > p_Top;

        if (overlapX && overlapY) {
            player->Hit(1);
        }
    }

    return;
}