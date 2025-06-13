//
// Created by MiHu on 6/13/2025.
//

#ifndef GASMACHINE_H
#define GASMACHINE_H
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"


class GasMachine : public  Engine::Sprite {
    PlayScene *getPlayScene();


public:
    GasMachine(std::string img, float x, float y);
    void Update(float deltaTime) override;
    bool IsCollision(float x, float y);

    bool active;
    float gasSpawnTimer;
    int currentGasIndex;
};



#endif //GASMACHINE_H
