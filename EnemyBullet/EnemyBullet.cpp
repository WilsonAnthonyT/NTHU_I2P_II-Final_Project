//
// Created by User on 09/06/2025.
//

#include "EnemyBullet.h"

#include <allegro5/allegro_primitives.h>

#include "Enemy/Enemy.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

PlayScene *EnemyBullet::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
// void EnemyBullet::OnExplode(Enemy *enemy) {
// }
EnemyBullet::EnemyBullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Sprite *parent) : Sprite(img, position.x, position.y), speed(speed), damage(damage), parent(parent) {
    Velocity = forwardDirection.Normalize() * speed;
    Rotation = rotation;
    CollisionRadius = 6;
    Anchor = Engine::Point(0.5,0.5);
}
void EnemyBullet::Update(float deltaTime) {
    auto scene = getPlayScene();
    Sprite::Update(deltaTime);
    if (IsCollision(Position.x, Position.y)) {
        scene->EnemyBulletGroup->RemoveObject(objectIterator);
        return;
    }
    if (!Engine::Collider::IsRectOverlap(Position - Size / 2, Position + Size / 2, Engine::Point(0, 0), PlayScene::GetClientSize()))
        scene->EnemyBulletGroup->RemoveObject(objectIterator);
}

bool EnemyBullet::IsCollision(float x, float y) {
    auto scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene) return true;

    // Calculate bullet's square hitbox with tolerance
    float halfSize_x = abs(Size.x)/2;
    float bulletLeft = Position.x - halfSize_x/ 2;
    float bulletRight = Position.x + halfSize_x / 2;
    float bulletTop = Position.y - abs(Size.y)/4;
    float bulletBottom = Position.y + abs(Size.y)/4;

    for (auto &it : scene->PlayerGroup->GetObjects()) {
        auto *player = dynamic_cast<Player *>(it);
        float PlayerHalfSizeX = abs(player->Size.x / 2);
        float PlayerLeft = player->Position.x - PlayerHalfSizeX;
        float PlayerRight = player->Position.x + PlayerHalfSizeX ;
        float PlayerTop = player->Position.y;
        float PlayerBottom = player->Position.y + abs(player->Size.y/2);

        if (!player->Visible)
            continue;
        if (PlayerLeft < bulletLeft && PlayerRight > bulletLeft && PlayerTop < bulletBottom && PlayerBottom > bulletTop) {
            player->Hit(damage);
            return true;
        }
    }

    // Screen boundaries
    Engine::Point MapBound = PlayScene::GetClientSize();
    float ScreenBound = PlayScene::Camera.x + Engine::GameEngine::GetInstance().GetScreenWidth();

    if (bulletLeft < 0 || bulletRight > MapBound.x ||
        bulletTop < 0 || bulletBottom > MapBound.y) {
        return true;
        }

    if (bulletLeft < PlayScene::Camera.x || bulletRight > ScreenBound) {
        return true;
    }

    // Tile collisions (ignore platforms)
    int leftTile = static_cast<int>(bulletLeft / PlayScene::BlockSize);
    int rightTile = static_cast<int>(bulletRight / PlayScene::BlockSize);
    int topTile = static_cast<int>(bulletTop / PlayScene::BlockSize);
    int bottomTile = static_cast<int>(bulletBottom / PlayScene::BlockSize);

    for (int yTile = topTile; yTile <= bottomTile; yTile++) {
        for (int xTile = leftTile; xTile <= rightTile; xTile++) {
            if (xTile >= 0 && xTile < scene->MapWidth && yTile >= 0 && yTile < scene->MapHeight) {
                // Only collide with dirt tiles, ignore platforms
                if (scene->mapState[yTile][xTile] == PlayScene::TILE_DIRT) {
                    return true;
                }
            }
        }
    }

    return false;
}

void EnemyBullet::Draw() const {
    Sprite::Draw();
    if (Engine::IScene::DebugMode) {
        float halfSize_x = abs(Size.x)/2;
        float bulletLeft = Position.x - halfSize_x/ 2;
        float bulletRight = Position.x + halfSize_x / 2;
        float bulletTop = Position.y - abs(Size.y)/4;
        float bulletBottom = Position.y + abs(Size.y)/4;
        al_draw_rectangle(bulletLeft, bulletTop, bulletRight, bulletBottom, al_map_rgb(0,0,0), 4);
        //al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255,155,0), 3);
    }
}
