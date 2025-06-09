#include <string>

#include "Engine/Sprite.hpp"
#include "Player/Player.h"

#ifndef ENEMY_BULLET_HPP
#define ENEMY_BULLET_HPP


class PlayScene;
namespace Engine {
    struct Point;
}   // namespace Engine

class EnemyBullet : public Engine::Sprite {
protected:
    float speed;
    float damage;
    Sprite *parent;
    PlayScene *getPlayScene();
    // virtual void OnExplode(Enemy *enemy);

public:
    Player *Target = nullptr;
    explicit EnemyBullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Sprite *parent);
    void Update(float deltaTime) override;
    bool IsCollision(float x, float y);
    void Draw() const override;
};
#endif   //ENEMY_BULLET_HPP
