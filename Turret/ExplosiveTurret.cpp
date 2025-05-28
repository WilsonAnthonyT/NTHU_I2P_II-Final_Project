#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/MissileBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "ExplosiveTurret.hpp"
#include "Scene/PlayScene.hpp"

const int ExplosiveTurret::Price = 350;
ExplosiveTurret::ExplosiveTurret(float x, float y) : Turret("play/tower-base.png", "play/turret-3.png", x, y, 400, Price, 1.35) {
    Anchor.y += 8.0f / GetBitmapHeight();
}

void ExplosiveTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    // Change bullet position to the front of the gun barrel.
    getPlayScene()->BulletGroup->AddNewObject(new MissileBullet(Position + normalized * 36, diff, rotation, this));
    AudioHelper::PlayAudio("explosion.wav");
}

