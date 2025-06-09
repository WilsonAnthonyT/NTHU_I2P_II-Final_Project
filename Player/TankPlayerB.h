//
// Created by User on 09/06/2025.
//

#ifndef TANKPLAYERB_H
#define TANKPLAYERB_H
#include "Player.h"

class TankPlayerB : public Player {
protected:
    float FireRate;
    float Cooldown;
    float bulletspeed;
public:
    TankPlayerB(float x, float y);
    void Update(float deltaTime) override;
};
#endif //TANKPLAYERB_H
