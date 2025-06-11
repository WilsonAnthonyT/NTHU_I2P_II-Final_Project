//
// Created by MiHu on 6/9/2025.
//

#include "Sensor.h"

#include <iostream>

#include "Box.h"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"

PlayScene *Sensor::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Sensor::Sensor(std::string img, float x, float y) : Engine::Sprite(img, x, y){
    active = false;
    Anchor = Engine::Point(0.5, -1);
    Size.x=PlayScene::BlockSize,Size.y=PlayScene::BlockSize;
    Bitmap = Engine::Resources::GetInstance().GetBitmap(img);
}

void Sensor::Update(float deltaTime) {
    auto scene = getPlayScene();
    if (!scene) return;

    bool nowActive = IsCollision(Position.x, Position.y) >= Weight;

    if (nowActive) {
        active = true;

        Sensor* sensorObj = dynamic_cast<Sensor*>(GetObjectIterator()->second);
        auto& doorList = scene->DoorSensorAssignments[sensorObj];

        for (auto door : doorList) {
            if (door->currState == door->initState) {
                door->currState = (door->initState == Door::OPEN)? Door::CLOSE : Door::OPEN;

                (door->currState == Door::OPEN)?
                    door->Tint = al_map_rgb(85, 55, 0) : door->Tint = al_map_rgb(255, 255, 255);
            }
        }

        bmp = Engine::Resources::GetInstance().GetBitmap("play/sensor-active.png");
    }
    else{
        active = false;
        bmp = Engine::Resources::GetInstance().GetBitmap("play/sensor.png");
        triggered = false;
    }


    if (Engine::IScene::DebugMode && active) {
        Tint=al_map_rgb(255,0,0);
    }
    else if (Engine::IScene::DebugMode && !active) Tint=al_map_rgb(255,255,255);
    else if (!Engine::IScene::DebugMode) Tint=al_map_rgb(255,255,255);
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

        if (overlapX && overlapY && !player->isFalling && !player->isJumping) {
            count++;
        }
    }

    for (auto& it : scene->InteractiveBlockGroup->GetObjects()) {
        Box* others = dynamic_cast<Box*>(it);
        // Skip if enemy is not visible, is the healer itself, or invalid
        if (!others || !others->Visible) continue;
        int half_P2 = abs(others->Size.x) / 2;

        float p2_Left = others->Position.x - half_P2;
        float p2_Right = others->Position.x + half_P2;
        float p2_Top = others->Position.y;
        float p2_Bottom = others->Position.y + others->Size.y;

        bool overlapX = left < p2_Right && right > p2_Left;
        bool overlapY = top < p2_Bottom && bottom > p2_Top;

        if (overlapX && overlapY) {
            count++;
        }
    }

    return count;
}
