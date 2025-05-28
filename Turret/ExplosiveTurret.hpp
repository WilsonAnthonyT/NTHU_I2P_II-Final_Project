//
// Created by User on 10/05/2025.
//
#ifndef EXPLOSIVETURRET_H
#define EXPLOSIVETURRET_H
#include "Turret.hpp"

class ExplosiveTurret : public Turret {
    public:
    static const int Price;
    ExplosiveTurret(float x, float y);
    void CreateBullet() override;
};

#endif //EXPLOSIVETURRET_H
