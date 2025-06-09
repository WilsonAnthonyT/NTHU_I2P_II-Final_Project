//
// Created by User on 09/06/2025.
//

#ifndef TANKPLAYERA_H
#define TANKPLAYERA_H
#include "Player.h"

class TankPlayerA : public Player {
protected:
    float FireRate;
    float Cooldown;
    float bulletspeed;
public:
    TankPlayerA(float x, float y);
    void Update(float deltaTime) override;
};
#endif //TANKPLAYERA_H
