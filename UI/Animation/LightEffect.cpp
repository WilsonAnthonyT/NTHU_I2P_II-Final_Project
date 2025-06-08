//
// Created by User on 12/05/2025.
//

#include <cmath>
#include <string>

#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Resources.hpp"
#include "LightEffect.hpp"
#include "Scene/PlayScene.hpp"

PlayScene *LightEffect::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
LightEffect::LightEffect(float x, float y, bool flipped) : Sprite("play/slash1.png", x, y), timeTicks(0), flipped(flipped) {
    for (int i = 2; i <= 5; i++) {
        bmps.push_back(Engine::Resources::GetInstance().GetBitmap("play/slash" + std::to_string(i) + ".png"));
    }
    Size = Engine::Point(PlayScene::BlockSize, PlayScene::BlockSize);
}
void LightEffect::Update(float deltaTime) {
    timeTicks += deltaTime * 2.0f;

    // Smooth scaling
    Size.x = PlayScene::BlockSize + timeTicks * 50.0f;
    Size.y = PlayScene::BlockSize + timeTicks * 50.0f;

    // Smooth alpha fade out
    alpha = std::max(0.0f, 1.0f - (timeTicks / timeSpan) * 0.85f);

    if (timeTicks >= timeSpan) {
        getPlayScene()->EffectGroup->RemoveObject(objectIterator);
        return;
    }

    // Smooth frame timing
    int totalFrames = bmps.size();
    float frameTime = timeSpan / totalFrames;
    int phase = std::min(static_cast<int>(timeTicks / frameTime), totalFrames - 1);
    bmp = bmps[phase];

    Sprite::Update(deltaTime);
}



void LightEffect::Draw() const {
    ALLEGRO_COLOR tintColor = al_map_rgba_f(1.0f, 1.0f, 1.0f, alpha);
    float drawX = Position.x - Anchor.x * Size.x;
    float drawY = Position.y - Anchor.y * Size.y;
    float scaleX = flipped ? -Size.x : Size.x;
    float originOffsetX = flipped ? Size.x : 0;

    al_draw_tinted_scaled_bitmap(
        bmp.get(),
        tintColor,
        0, 0,
        al_get_bitmap_width(bmp.get()),
        al_get_bitmap_height(bmp.get()),
        drawX + originOffsetX, // Adjust to keep anchor consistent when flipped
        drawY,
        scaleX,
        Size.y,
        0
    );
}


