//
// Created by MiHu on 6/10/2025.
//

#ifndef PORTAL_H
#define PORTAL_H
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"


class Portal : public Engine::Sprite {
protected:
    PlayScene *getPlayScene();
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> ActiveAnimation;
    float animationTime = 0;
    float frameDuration = 0.75f; // Time between frames in seconds
    int currentFrame = 0;

public:
    Portal(std::string img, float x, float y);
    void Update(float deltaTime) override;
    void UpdateAnimation(float deltaTime);
    void IsCollision(float x, float y);
    std::shared_ptr<ALLEGRO_BITMAP> Bitmap;

    bool active;
};



#endif //PORTAL_H
