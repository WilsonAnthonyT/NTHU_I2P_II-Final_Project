//
// Created by MiHu on 6/9/2025.
//

#ifndef MAZEPLAYERB_H
#define MAZEPLAYERB_H
#include "Player.h"


class MazePlayerB : public Player {
public:
    MazePlayerB(float x, float y);
    void Update(float deltaTime) override;
    //void Draw() const override;
};



#endif //MAZEPLAYERB_H
