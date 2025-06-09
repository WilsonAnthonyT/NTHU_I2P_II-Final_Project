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
    float TintTimer;
    PlayScene *Play;
    float jumpForce;
    bool goDown;
    float goDownTimer = 0.0f;
    float tolerance;
    float armor;
public:
    float hp;
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

    void PlayerEnemyCollision(Player* player, float time);
    void Hit(float damage);
    bool isKnockedback = false;
    float knockbackVelocityX = 0;
    float knockbackTimer = 0;
    const float maxKnockbackTime = 0.5f;
};



#endif //PLAYER_H
