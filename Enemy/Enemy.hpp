#ifndef ENEMY_HPP
#define ENEMY_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/AudioHelper.hpp"

class Bullet;
class PlayScene;

class Enemy : public Engine::Sprite {
protected:
    std::vector<Engine::Point> path;
    float speed;
    float hp;
    int money;
    //for scores
    int scores;
    bool boostedsoundplayed;
    PlayScene *getPlayScene();
    virtual void OnExplode();
    float MaxHp;
    float TintTimer;
    float verticalVelocity;
    bool isJumping;
    float jumpForce;
    bool flipped;
    bool isFalling;
    bool goDown;
    float goDownTimer = 0.0f;
    float tolerance;
    float VelocityX;
    float damage;
    float AttackRange = 0.0f;


    //for animation
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> idleAnimation;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> walkAnimation;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> attackingAnimation;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> deathAnimation;

    float animationTime = 0;
    int currentFrame = 0;
    float idleFrameDuration = 0.15f;
    float walkFrameDuration = 0.1f;
    float attackFrameDuration = 0.08f;
    float deathFrameDuration = 0.12f;
    bool isAttacking = false;
    enum EnemyState {
        IDLE,
        WALKING,
        ATTACKING,
        DEAD
    };
    EnemyState currentState = IDLE;

public:
    int getDamage(){return damage;};
    Engine::Point BFSPathfind(const Engine::Point& target);
    float reachEndTime;
    std::list<Bullet *> lockedBullets;
    Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money, int scores, float dmg);
    virtual void OnDeath();
    virtual void Hit(float damage, float PosX, std::string type);

    virtual void ChasePlayer(const std::vector<Engine::Point>& playerPositions, float deltaTime);

    void Update(float deltaTime) override;
    void Draw() const override;
    virtual bool IsCollision(float x, float y, bool checkWallsOnly);
    bool IsPlayerInRange(float x, float y, float AttackRange);


    //I add this
    float getHp() const;
    float getSpeed() const;
    void SetSpeed(float speed);

    //for knockback
    float knockbackVelocityX = 0;
    float knockbackTimer = 0;
    const float maxKnockbackTime = 0.2f; // 200 ms knockback duration
    bool isKnockedback = false;
    bool HasBeenHitThisSwing = false;

    //so that the enemy only attack the player when in frame
    bool IsInCameraView(float x, float y);
    bool startChase = false;

    virtual void UpdateAnimation(float deltaTime);

};
#endif   // ENEMY_HPP
