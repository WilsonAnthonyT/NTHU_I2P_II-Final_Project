//
// Created by MiHu on 6/14/2025.
//

#ifndef DORAEMON_H
#define DORAEMON_H
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"


class Doraemon : public Engine::Sprite{
protected:
    PlayScene *getPlayScene();

public:
    Doraemon(std::string img, float x, float y);
    void Update(float deltaTime) override;
    bool IsCollision(float x, float y);
};



#endif //DORAEMON_H
