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
    int Weight;


public:
    Sensor(std::string img, float x, float y, int weight);
    void Update(float deltaTime) override;
    void Draw() const override;
    int IsCollision(float x, float y);
    std::shared_ptr<ALLEGRO_BITMAP> Bitmap;

    bool active;;
};



#endif //SENSOR_H
