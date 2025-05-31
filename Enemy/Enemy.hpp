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
    bool boosted;
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

public:
    Engine::Point BFSPathfind(const Engine::Point& target);
    float reachEndTime;
    std::list<Bullet *> lockedBullets;
    Enemy(std::string img, float x, float y, float radius, float speed, float hp, int money, int scores, bool boosted);
    virtual void OnDeath();
    void Hit(float damage, float PosX);

    void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
    void ChasePlayer(const std::vector<Engine::Point>& playerPositions, float deltaTime);

    void Update(float deltaTime) override;
    void Draw() const override;
    bool IsCollision(float x, float y, bool checkWallsOnly);


    //I add this
    float getHp() const;
    float getSpeed() const;
    void SetSpeed(float speed);

    //for boosterEnemy
    bool isBoosted() const;
    void setBoosted(bool boosted);
    void transform();
    static void getHitbox();

    //for knockback
    float knockbackVelocityX = 0;
    float knockbackTimer = 0;
    const float maxKnockbackTime = 0.2f; // 200 ms knockback duration
    bool isKnockedback = false;

};
#endif   // ENEMY_HPP
