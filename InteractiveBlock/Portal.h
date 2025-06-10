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

public:
    Portal(std::string img, float x, float y);
    void Update(float deltaTime) override;
    void IsCollision(float x, float y);
    std::shared_ptr<ALLEGRO_BITMAP> Bitmap;

    bool active;
};



#endif //PORTAL_H
