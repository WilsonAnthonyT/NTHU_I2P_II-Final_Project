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
    bool movementEnabled = true;
    int TintCounter = 0;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> idleAnimation;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> walkAnimation;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> jumpAnimation;
    float animationTime = 0;
    float frameDuration = 0.75f; // Time between frames in seconds
    int currentFrame = 0;

    enum PlayerState {
        IDLE,
        WALKING,
        JUMPING,
        FALLING
    };
    PlayerState currentState = IDLE;

public:
    float hp;
    float speed;
    float verticalVelocity;
    bool isJumping;
    bool isFalling;
    bool flipped;

    PlayScene *getPlayScene();

    Player(std::string img, float x, float y, float speed, float hp);
    void Update(float deltaTime) override;
    void Draw() const override;
    bool IsCollision(float x, float y);
    bool InteractiveBlockCollision(float x, float y);
    void remove();

    void PlayerEnemyCollision(Player* player, float time);
    void PlayerBulletCollision(Player* player, float time);
    void Hit(float damage);

    bool isKnockedback = false;
    float knockbackVelocityX = 0;
    float knockbackTimer = 0;
    const float maxKnockbackTime = 0.5f;

    bool isBulletKnockback = false;
    float bulletKnockbackVelocityX = 0;
    float bulletKnockbackVelocityY = 0;
    float bulletKnockbackTimer = 0;
    const float maxBulletKnockbackTime = 0.5f;

    //for movement during dialogue
    void EnableMovement(bool enable) { movementEnabled = enable; }
    bool IsMovementEnabled() const { return movementEnabled; }

};



#endif //PLAYER_H
