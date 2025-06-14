#include <allegro5/base.h>
#include <random>
#include <string>

#include "Enemy/Enemy.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "FireBullet.hpp"

#include "Player/RangePlayer.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Player;

FireBullet::FireBullet(Engine::Point position, Engine::Point forwardDirection, float rotation, Sprite *parent, float speed) : Bullet("play/bullet-1.png", speed, 5, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
}
void FireBullet::OnExplode(Enemy *enemy) {
    // std::random_device dev;
    // std::mt19937 rng(dev());
    // std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    // getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}

