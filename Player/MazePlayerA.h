//
// Created by MiHu on 6/9/2025.
//

#ifndef MAZEPLAYERA_H
#define MAZEPLAYERA_H
#include "Player.h"
#include "Scene/PlayScene.hpp"


class MazePlayerA : public Player {
public:
    float visibilityRadius = PlayScene::BlockSize * 2.0f;
    MazePlayerA(float x, float y);
    void Update(float deltaTime) override;
    //void Draw() const override;
};



#endif //MAZEPLAYERA_H
