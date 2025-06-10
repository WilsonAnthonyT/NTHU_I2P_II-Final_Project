//
// Created by benef on 10/06/2025.
//

#ifndef DOOR_H
#define DOOR_H

#include <vector>
#include <unordered_map>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/AudioHelper.hpp"

class Bullet;
class PlayScene;

class Door : public Engine::Sprite {
protected:
    PlayScene *getPlayScene();
    float verticalVelocity = 0;
public:
    Door(std::string img, float x, float y);
    void Update(float deltaTime) override;
    bool IsCollision(float x, float y);
};

#endif //DOOR_H
