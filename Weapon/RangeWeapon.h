#ifndef RANGEWEAPON_H
#define RANGEWEAPON_H

#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class PlayScene;
class Player;

class RangeWeapon : public Engine::Sprite {
protected:
    Player *player;
    const float FireRate;
    int magazine;
    bool flipped;
    PlayScene *Play;
    float Cooldown;
    const float speed;
public:
    RangeWeapon(std::string img, float x, float y, float fr, int magazine,Player *player, float speed);
    void Update(float deltaTime) override;
    void Draw() const override;
};



#endif //RANGEWEAPON_H
