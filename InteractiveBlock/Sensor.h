//
// Created by MiHu on 6/9/2025.
//

#ifndef SENSOR_H
#define SENSOR_H
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"


class Sensor : public Engine::Sprite{
protected:
    PlayScene *getPlayScene();

public:
    int Weight;
    Sensor(std::string img, float x, float y);
    void Update(float deltaTime) override;
    void Draw() const override;
    int IsCollision(float x, float y);

    bool active;
};



#endif //SENSOR_H
