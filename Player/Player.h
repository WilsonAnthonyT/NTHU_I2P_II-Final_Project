#ifndef PLAYER_H
#define PLAYER_H

#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class PlayScene;

class Player : public Engine::Sprite{
protected:
    float MaxHp;
    float speed;
    float hp;
    float TintTimer;
    PlayScene *Play;

public:
    Player(std::string img, float x, float y, float radius, float speed, float hp);
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnKeyDown(int keyCode);
};



#endif //PLAYER_H
