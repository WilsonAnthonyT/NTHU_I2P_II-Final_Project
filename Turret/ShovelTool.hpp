//
// Created by User on 11/05/2025.
//

#ifndef SHOVELTOOL_HPP
#define SHOVELTOOL_HPP
#include "Turret.hpp"

class ShovelTool : public Turret {
    public:
    static const int Price;
    ShovelTool(float x, float y);
    void OnMouseDown(int gridX, int gridY);
    void CreateBullet() override;
};
#endif //SHOVELTOOL_HPP
