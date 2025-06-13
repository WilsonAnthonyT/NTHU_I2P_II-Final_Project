//
// Created by MiHu on 6/12/2025.
//

#ifndef SWORDSKELLY_H
#define SWORDSKELLY_H
#include "Enemy.hpp"

class SwordSkelly : public Enemy {
    bool isAttacking = false;
    float attackCooldown = 0.0f;
    const float attackInterval = 1.5f;
public:
    SwordSkelly(int x, int y);
    void Update(float deltaTime) override;
    void UpdateAnimation(float deltaTime)override;
};


#endif //SWORDSKELLY_H
