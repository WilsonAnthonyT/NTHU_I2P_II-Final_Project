#ifndef BOX_H
#define BOX_H

#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/AudioHelper.hpp"

class Bullet;
class PlayScene;

class Box : public Engine::Sprite {
protected:
    PlayScene *getPlayScene();
    float verticalVelocity = 0;
public:
    Box(std::string img, float x, float y);
    void Update(float deltaTime) override;
    bool IsCollision(float x, float y);

    bool Push(float x, float y);

    float horizontalVelocity = 0;
};

#endif //BOX_H
