//
// Created by User on 12/05/2025.
//

#include <cmath>
#include <string>

#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Resources.hpp"
#include "LightEffect.h"
#include "Scene/PlayScene.hpp"

PlayScene *LightEffect::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
LightEffect::LightEffect(float x, float y) : Sprite("play/explosion-1.png", x, y), timeTicks(0) {
    for (int i = 1; i <= 10; i++) {
        bmps.push_back(Engine::Resources::GetInstance().GetBitmap("play/light-" + std::to_string(i) + ".png"));

    }
    Size = Engine::Point(300.0f, 300.0f);
}
void LightEffect::Update(float deltaTime) {
    timeTicks += deltaTime * 0.7f;

    Size.x += deltaTime * 50.0f;
    Size.y += deltaTime * 50.0f;

    if (timeTicks >= timeSpan) {
        getPlayScene()->EffectGroup->RemoveObject(objectIterator);
        return;
    }
    int phase = floor(timeTicks / timeSpan * bmps.size());
    bmp = bmps[phase];
    Sprite::Update(deltaTime);
}
