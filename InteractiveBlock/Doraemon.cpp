//
// Created by MiHu on 6/14/2025.
//

#include "../InteractiveBlock/Doraemon.h"

#include "Engine/GameEngine.hpp"

PlayScene *Doraemon::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Doraemon::Doraemon(std::string img, float x, float y) : Sprite(img, x, y) {
    Anchor = Engine::Point(0.5, 0);
    Size.x = PlayScene::BlockSize, Size.y = PlayScene::BlockSize;
}

void Doraemon::Update(float deltaTime) {
    auto scene = getPlayScene();
    if (!scene) return;

    // Check if player is colliding to collect
    if (IsCollision(Position.x, Position.y)) {

    }
}

bool Doraemon::IsCollision(float x, float y) {
    PlayScene* scene = getPlayScene();
    if (!scene) return false;

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
            return true;
        }
    }
    return false;
}