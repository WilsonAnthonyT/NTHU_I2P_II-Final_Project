//
// Created by User on 14/05/2025.
//

#ifndef MEDICTOOL_H
#define MEDICTOOL_H
#include "Turret.hpp"

class MedicTool : public Turret {
public:
    static const int Price;
    MedicTool(float x, float y);
    void OnMouseDown(int gridX, int gridY);
    void CreateBullet() override;
};



#endif //MEDICTOOL_H
