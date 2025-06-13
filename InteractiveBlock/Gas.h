//
// Created by MiHu on 6/13/2025.
//

#ifndef GAS_H
#define GAS_H
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"


class Gas : public Engine:: Sprite {
    PlayScene *getPlayScene();
    float gasTick = 0.0f;
public:
    Gas(std::string img, float x, float y);
    void Update(float deltaTime) override;
    void IsCollision(float x, float y);
};



#endif //GAS_H
