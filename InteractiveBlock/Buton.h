//
// Created by MiHu on 6/10/2025.
//

#ifndef BUTON_H
#define BUTON_H
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"


class Buton : public Engine::Sprite{
protected:
    PlayScene *getPlayScene();

public:
    Buton(std::string img, float x, float y);
    void Update(float deltaTime) override;
    bool IsCollision(float x, float y);

    bool active;
};



#endif //BUTON_H
