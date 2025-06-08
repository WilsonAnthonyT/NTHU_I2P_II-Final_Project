//
// Created by User on 12/05/2025.
//

#ifndef LIGHTEFFECT_H
#define LIGHTEFFECT_H
#include <allegro5/bitmap.h>
#include <memory>
#include <vector>

#include "Engine/Sprite.hpp"

class PlayScene;

class SlashEffect : public Engine::Sprite {
protected:
    PlayScene *getPlayScene();
    float timeTicks;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> bmps;
    float timeSpan = 0.5;
    float alpha = 1.0f; // Full opacity by default
    bool flipped;

public:
    SlashEffect(float x, float y, bool flipped);
    void Update(float deltaTime) override;
    void Draw() const override;
};



#endif //LIGHTEFFECT_H
