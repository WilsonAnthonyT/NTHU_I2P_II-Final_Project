#ifndef FLYINGENEMY
#define FLYINGENEMY

#include "Enemy.hpp"

class FlyingEnemy : public Enemy {
protected:
    bool isFlying;
    float VelocityY;
    float knockbackVelocityY;
    
public:
    FlyingEnemy(std::string img, float x, float y, float radius, float speed, float hp, int money, int scores, float dmg);
    
    void Update(float deltaTime) override;
    void ChasePlayer(const std::vector<Engine::Point>& playerPositions, float deltaTime) override;
    void Hit(float damage, float PosX, std::string type) override;
    bool IsCollision(float x, float y, bool checkWallsOnly) override;
    void Draw() const override;
};

#endif // FLYINGENEMY