//
// Created by User on 12/05/2025.
//

#ifndef BOOSTERENEMY_HPP
#define BOOSTERENEMY_HPP
#include "Enemy.hpp"

class BoosterEnemy : public Enemy {
    public:
    BoosterEnemy(int x, int y);
    void applyBooster(Enemy *enemy);
    void Draw() const override;
    float boostRadius = CollisionRadius / 2;
};

#endif //BOOSTERENEMY_HPP
