#include <allegro5/allegro.h>
#include <cmath>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Plane.hpp"
#include "Scene/PlayScene.hpp"

PlayScene *Plane::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
Plane::Plane() : Sprite("play/plane.png", -100, Engine::GameEngine::GetInstance().GetScreenHeight() / 2), stage(0), timeTicks(0) {
    for (int i = 1; i <= 10; i++) {
        bmps.push_back(Engine::Resources::GetInstance().GetBitmap("play/light-" + std::to_string(i) + ".png"));
    }
    shockwave = Engine::Resources::GetInstance().GetBitmap("play/shockwave.png");
    Velocity = Engine::Point(1600, 0);

    const float sizeMultiplier = 1.8f; // 1.8x bigger
    Size.x = GetBitmapWidth() * sizeMultiplier;
    Size.y = GetBitmapHeight() * sizeMultiplier;
}
void Plane::Update(float deltaTime) {

}
void Plane::Draw() const {
    // FIXME: known issue
    // On MacOS, the explosion effect cannot be drawn correctly.
    // If you know how to fix it, please let us know.
    unsigned int phase = floor(timeTicks / timeSpanLight * bmps.size());
    float phaseRatio = timeTicks / timeSpanLight * bmps.size() - phase;
    if (stage == 1 || stage == 2) {
        if (phase > 7 && phase < bmps.size()) {
            al_draw_tinted_scaled_rotated_bitmap(bmps[phase - 7].get(), al_map_rgba(0, 0, 255, 255 - phaseRatio * 64), Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
                                                 Position.x, Position.y, Size.x / GetBitmapWidth(), Size.y / GetBitmapHeight(), Rotation, 0);
        }
        if (phase > 6 && phase < bmps.size()) {
            al_draw_tinted_scaled_rotated_bitmap(bmps[phase - 6].get(), al_map_rgba(0, 0, 255, 191 - phaseRatio * 64), Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
                                                 Position.x, Position.y, Size.x / GetBitmapWidth(), Size.y / GetBitmapHeight(), Rotation, 0);
        }
        if (phase > 5 && phase < bmps.size()) {
            al_draw_tinted_scaled_rotated_bitmap(bmps[phase - 5].get(), al_map_rgba(0, 255, 0, 127 - phaseRatio * 64), Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
                                                 Position.x, Position.y, Size.x / GetBitmapWidth(), Size.y / GetBitmapHeight(), Rotation, 0);
        }
        if (phase > 4 && phase < bmps.size()) {
            al_draw_tinted_scaled_rotated_bitmap(bmps[phase - 4].get(), al_map_rgba(255, 255, 255, phaseRatio * 63), Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
                                                 Position.x, Position.y, Size.x / GetBitmapWidth(), Size.y / GetBitmapHeight(), Rotation, 0);
        }
        if (phase > 3 && phase < bmps.size()) {
            al_draw_tinted_scaled_rotated_bitmap(bmps[phase - 3].get(), al_map_rgba(255, 0, 0, 63 - phaseRatio * 63), Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
                                                 Position.x, Position.y, Size.x / GetBitmapWidth(), Size.y / GetBitmapHeight(), Rotation, 0);
        }
        if (phase > 2 && phase < bmps.size()) {
            al_draw_tinted_scaled_rotated_bitmap(bmps[phase - 2].get(), al_map_rgba(255, 0, 0, 127 - phaseRatio * 64), Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
                                                 Position.x, Position.y, Size.x / GetBitmapWidth(), Size.y / GetBitmapHeight(), Rotation, 0);
        }
        if (phase > 1 && phase < bmps.size()) {
            al_draw_tinted_scaled_rotated_bitmap(bmps[phase - 1].get(), al_map_rgba(0, 255, 0, 191 - phaseRatio * 64), Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
                                                 Position.x, Position.y, Size.x / GetBitmapWidth(), Size.y / GetBitmapHeight(), Rotation, 0);
        }
    }
    if (stage == 1)
        al_draw_tinted_scaled_rotated_bitmap(bmp.get(), al_map_rgba(255, 255, 255, 255 - phaseRatio * 64), Anchor.x * GetBitmapWidth(), Anchor.y * GetBitmapHeight(),
                                             Position.x, Position.y, Size.x / GetBitmapWidth(), Size.y / GetBitmapHeight(), Rotation, 0);
    else {
        Sprite::Draw();
    }
}
