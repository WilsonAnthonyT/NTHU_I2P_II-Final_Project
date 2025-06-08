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
    float speed;
    float hp;
    float TintTimer;
    PlayScene *Play;
    float verticalVelocity;
    bool isJumping;
    float jumpForce;
    bool isFalling;
    bool goDown;
    float goDownTimer = 0.0f;
    float tolerance;
    float armor;
public:
    bool flipped;
    Player(std::string img, float x, float y, float speed, float hp);
    void Update(float deltaTime) override;
    void Draw() const override;
    void Hit(float damage, float PosX);
    bool IsCollision(float x, float y);
};



#endif //PLAYER_H
