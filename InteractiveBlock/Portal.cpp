//
// Created by MiHu on 6/10/2025.
//

#include "Portal.h"

#include "Buton.h"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include <iostream>

PlayScene *Portal::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Portal::Portal(std::string img, float x, float y) : Sprite(img, x, y){
    active = false;
    Anchor = Engine::Point(0.5,0.5);
    Size.x=PlayScene::BlockSize,Size.y=PlayScene::BlockSize;
    Bitmap = Engine::Resources::GetInstance().GetBitmap(img);
    for (int i = 1; i <= 6; i++) {
        ActiveAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/portal-active-" + std::to_string(i) + ".png"));
    }
}

void Portal::Update(float deltaTime) {
    int total = 0,count = 0;
    PlayScene* scene = getPlayScene();
    for (auto& it : scene->InteractiveBlockGroup->GetObjects()) {
        Buton* buton = dynamic_cast<Buton*>(it);
        // Skip if enemy is not visible, is the healer itself, or invalid
        if (!buton || !buton->Visible) continue;
        total++;
        if (buton->active)count++;
    }
    if (total == count) {
        active=true;
    }
    if (active)IsCollision(Position.x,Position.y);

    UpdateAnimation(deltaTime);
}

void Portal::IsCollision(float x, float y) {
    PlayScene* scene = getPlayScene();
    if (!scene) return;

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
            player->Visible = false;
        }
    }
}

void Portal::UpdateAnimation(float deltaTime) {
    if (active) {
        animationTime += deltaTime;

        std::vector<std::shared_ptr<ALLEGRO_BITMAP>>* currentAnimation = nullptr;
        currentAnimation = &ActiveAnimation;
        frameDuration = 0.15f;

        if (animationTime >= frameDuration) {
            animationTime = 0;
            currentFrame = (currentFrame + 1) % currentAnimation->size();
            this->bmp = (*currentAnimation)[currentFrame]; // .get() to access raw pointer
        }
    }
    else {
        this->bmp = Engine::Resources::GetInstance().GetBitmap("play/portal.png");
    }
}
