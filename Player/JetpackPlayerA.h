//
// Created by User on 13/06/2025.
//

#ifndef JETPACKPLAYERA_H
#define JETPACKPLAYERA_H

#include "Player.h"
#include "Weapon/RangeWeapon.h"

class JetpackPlayerA : public Player {
public:
    JetpackPlayerA(float x, float y);
    void Update(float deltaTime) override;
    RangeWeapon *weapon;
    float jetpackForce = 500.0f;          // Upward force when jetpack is active
    float maxJetpackFuel = 3.0f;          // Maximum fuel in seconds
    float jetpackFuel = maxJetpackFuel;   // Current fuel
    float fuelRechargeRate = 0.5f;        // Fuel recharge rate per second
    float fuelRechargeDelay = 1.0f;       // Delay before recharging starts after last use
    float fuelRechargeTimer = 0.0f;       // Timer for recharging delay
    float gravityScale = 1.0f;            // Normal gravity when not using jetpack
    float horizontalSpeed = 300.0f;       // Horizontal movement speed
    float fallSpeed = 400.0f;
};

#endif //JETPACKPLAYERA_H
