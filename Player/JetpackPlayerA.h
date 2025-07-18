//
// Created by User on 13/06/2025.
//

#ifndef JETPACKPLAYERA_H
#define JETPACKPLAYERA_H

#include "Player.h"
#include "Scene/PlayScene.hpp"
#include "Weapon/RangeWeapon.h"

class JetpackPlayerA : public Player {
public:
    JetpackPlayerA(float x, float y);
    void Update(float deltaTime) override;
    RangeWeapon *weapon;
    float jetpackForce = PlayScene::BlockSize * 5;          // Upward force when jetpack is active
    float maxJetpackFuel = 10.0f;          // Maximum fuel in seconds
    float jetpackFuel = maxJetpackFuel;   // Current fuel
    float fuelRechargeRate = 0.5f;        // Fuel recharge rate per second
    float fuelRechargeDelay = 1.0f;       // Delay before recharging starts after last use
    float fuelRechargeTimer = 0.0f;       // Timer for recharging delay
    float gravityScale = 1.0f;            // Normal gravity when not using jetpack
    float horizontalSpeed = PlayScene::BlockSize * 2;       // Horizontal movement speed
    float fallSpeed = PlayScene::BlockSize * 3;

    void Draw() const override;
};

#endif //JETPACKPLAYERA_H
