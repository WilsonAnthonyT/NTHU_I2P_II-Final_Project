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
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> ActiveAnimation;
    float animationTime = 0;
    float frameDuration = 0.75f; // Time between frames in seconds
    int currentFrame = 0;
public:
    enum DoorState {
        OPEN,
        CLOSE,
    };
    DoorState currState, initState;
    bool horizontal;
    Door(std::string, float, float, DoorState,bool horizontal);
    void Update(float deltaTime) override;
    void UpdateAnimation(float deltaTime);
    std::shared_ptr<ALLEGRO_BITMAP> Bitmap;

};

#endif //DOOR_H
