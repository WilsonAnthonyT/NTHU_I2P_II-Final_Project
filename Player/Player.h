#ifndef PLAYER_H
#define PLAYER_H

#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class PlayScene;

class Player : public Engine::Sprite{
protected:
    float MaxHp;
    float hp;
    float TintTimer;
    PlayScene *Play;
    float jumpForce;
    bool goDown;
    float goDownTimer = 0.0f;
    float tolerance;
    float armor;
public:
    float speed;
    float verticalVelocity;
    bool isJumping;
    bool isFalling;
    bool flipped;
    Player(std::string img, float x, float y, float speed, float hp);
    void Update(float deltaTime) override;
    void Draw() const override;

    bool IsCollision(float x, float y);
    bool InteractiveBlockCollision(float x, float y);

    void push();
};



#endif //PLAYER_H
