//
// Created by MiHu on 6/9/2025.
//

#ifndef MAZEPLAYERA_H
#define MAZEPLAYERA_H
#include "Player.h"


class MazePlayerA : public Player {
public:
    MazePlayerA(float x, float y);
    void Update(float deltaTime) override;
};



#endif //MAZEPLAYERA_H
