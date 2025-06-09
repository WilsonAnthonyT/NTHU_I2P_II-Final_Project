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

class LightEffect : public Engine::Sprite {
protected:
    PlayScene *getPlayScene();
    float timeTicks;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> bmps;
    float timeSpan = 0.5;

public:
    LightEffect(float x, float y);
    void Update(float deltaTime) override;
};



#endif //LIGHTEFFECT_H
