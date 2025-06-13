#include "Player.h"

#include <iostream>
#include <allegro5/allegro_primitives.h>

#include "JetpackPlayerA.h"
#include "JetpackPlayerB.h"
#include "Enemy/Enemy.hpp"
#include "EnemyBullet/EnemyBullet.h"
#include "Engine/IScene.hpp"
#include "Engine/Collider.hpp"

#include "Engine/GameEngine.hpp"
#include "Engine/Sprite.hpp"
#include "InteractiveBlock/Box.h"
#include "InteractiveBlock/Portal.h"
#include "Scene/PlayScene.hpp"
PlayScene *Player::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
Player::Player(std::string img, float x, float y, float speed, float hp): Sprite(img,x,y),speed(speed), hp(hp),flipped(false){
    MaxHp = hp;
    TintTimer = 0.0f;
    verticalVelocity = 0.0f;
    isJumping = false;
    isFalling = false;
    goDown = false;
    // Calculate jump force for exactly 1.5 blocks (96 pixels) high
    jumpForce = sqrt(2 * PlayScene::Gravity * ((PlayScene::BlockSize)/2)*3); // 192 pixels/s
    Anchor = Engine::Point(0.5, 0);
    tolerance = 1.0 / 64.0 * PlayScene::BlockSize;
    armor = 0.0f;
}

void Player::Update(float deltaTime) {

}

void Player::Draw() const {
    Sprite::Draw();
    if (Engine::IScene::DebugMode) {
        const float healthBarWidth = abs(Size.x*3/4);
        const float healthBarHeight = PlayScene::BlockSize/15; // Height of the health bar
        const float healthBarOffset = PlayScene::BlockSize/6.4; // Offset above the enemy

        float healthBarX = Position.x - healthBarWidth/2;
        float healthBarY = Position.y - Size.y/2 + healthBarOffset;

        al_draw_filled_rectangle(healthBarX, healthBarY,
                                healthBarX + healthBarWidth, healthBarY + healthBarHeight,
                                al_map_rgb(0, 0, 0)); // Red background

        float healthRatio = static_cast<float>(hp) / static_cast<float>(MaxHp);
        al_draw_filled_rectangle(healthBarX, healthBarY,
                                healthBarX + healthBarWidth * healthRatio, healthBarY + healthBarHeight,
                                al_map_rgb(255, 0, 0)); // Green health

        float halfSize_x = abs(Size.x) / 2;
        //std::cout << "SIZE PLAYER X: " << Size.x / 2 << " FROM TILESIZE " << PlayScene::BlockSize << " to " << PlayScene::BlockSize *0.7 /2<< std::endl;

        float playerLeft = Position.x - halfSize_x;
        float playerRight = Position.x + halfSize_x;
        float playerTop = abs(Position.y);
        float playerBottom = Position.y + Size.y - tolerance;
        al_draw_rectangle(playerLeft, playerTop, playerRight, playerBottom, al_map_rgb(255, 0, 0), 2.0);
    }
}

bool Player::IsCollision(float x, float y) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene) return true;

    // Calculate player's square hitbox with 1-pixel tolerance
    float halfSize_x = abs(Size.x) / 2;
    //std::cout << "SIZE PLAYER X: " << Size.x / 2 << " FROM TILESIZE " << PlayScene::BlockSize << " to " << PlayScene::BlockSize *0.7 /2<< std::endl;

    float playerLeft = x - halfSize_x + tolerance;
    float playerRight = x + halfSize_x - tolerance;
    float playerTop = y + tolerance;
    float playerBottom = y + Size.y - tolerance;

    for (auto& it : scene->PlayerGroup->GetObjects()) {
        Player* friends = dynamic_cast<Player*>(it);
        // Skip if enemy is not visible, is the healer itself, or invalid
        if (!friends || !friends->Visible || friends == this) continue;
        int half_P2 = abs(friends->Size.x) / 2;

        float p2_Left = friends->Position.x - half_P2;
        float p2_Right = friends->Position.x + half_P2;
        float p2_Top = friends->Position.y;
        float p2_Bottom = friends->Position.y + friends->Size.y;

        bool overlapX = playerLeft < p2_Right && playerRight > p2_Left;
        bool overlapY = playerTop < p2_Bottom && playerBottom > p2_Top;

        if (overlapX && overlapY) return true;
    }

    // Screen boundaries
    Engine::Point MapBound = PlayScene::GetClientSize();
    Engine::Point ScreenBound{PlayScene::Camera.x + Engine::GameEngine::GetInstance().GetScreenWidth(), PlayScene::Camera.y + Engine::GameEngine::GetInstance().GetScreenHeight()};

    if (playerLeft < 0 || playerRight > MapBound.x ||
        playerTop < 0 || playerBottom > MapBound.y) {
        return true;
    }

    if (playerLeft < PlayScene::Camera.x ||
        playerRight > ScreenBound.x ||
        playerTop < PlayScene::Camera.y ||
        playerBottom > ScreenBound.y) {
        return true;
    }

    // Ground collisions
    int leftTile = static_cast<int>(playerLeft / PlayScene::BlockSize);
    int rightTile = static_cast<int>(playerRight / PlayScene::BlockSize);
    int topTile = static_cast<int>(playerTop / PlayScene::BlockSize);
    int bottomTile = static_cast<int>(playerBottom / PlayScene::BlockSize);

    for (int yTile = topTile; yTile <= bottomTile; yTile++) {
        for (int xTile = leftTile; xTile <= rightTile; xTile++) {
            if (xTile >= 0 && xTile < scene->MapWidth && yTile >= 0 && yTile < scene->MapHeight) {
                if (scene->mapState[yTile][xTile] == PlayScene::TILE_DIRT) {
                    return true;
                }
                // Special handling for platforms
                if (scene->mapState[yTile][xTile] == PlayScene::TILE_WPLATFORM) {
                    JetpackPlayerA *A = dynamic_cast<JetpackPlayerA*>(this);
                    JetpackPlayerB *B = dynamic_cast<JetpackPlayerB*>(this);
                    if ((A && A->verticalVelocity < 0) ||
                    (B && B->verticalVelocity < 0)) {
                        return false; // Skip this platform tile
                    }
                    // Only collide if player's bottom is touching the platform's top
                    float platformTop = yTile * PlayScene::BlockSize;

                    // Check if player's bottom is within the top few pixels of the platform
                    const float platformCollisionThreshold = PlayScene::BlockSize/8; // Adjust this value as needed
                    if (playerBottom >= platformTop &&
                        playerBottom <= platformTop + platformCollisionThreshold &&
                        playerRight > xTile * PlayScene::BlockSize &&
                        playerLeft < (xTile + 1) * PlayScene::BlockSize&&(!isJumping||isFalling)&&!goDown) {
                        return true;
                    }
                }
            }
        }
    }

    for (auto& obj : scene->InteractiveBlockGroup->GetObjects()) {
        Box* block = dynamic_cast<Box*>(obj);
        Door* dor = dynamic_cast<Door*>(obj);
        if ((block && block->Visible) || (dor && dor->Visible && dor->currState == Door::CLOSE)) {
            float objleft;
            float objright;
            float objtop;
            float objbottom;
            
            if (block) {
                objleft = block->Position.x - block->Size.x/2 + PlayScene::BlockSize/16;
                objright = block->Position.x + block->Size.x/2 - PlayScene::BlockSize/16;
                objtop = block->Position.y;
                objbottom = block->Position.y + block->Size.y;
            }
            if (dor) {
                objleft = dor->Position.x - dor->Size.x/2 + PlayScene::BlockSize/16;
                objright = dor->Position.x + dor->Size.x/2 - PlayScene::BlockSize/16;
                objtop = dor->Position.y - dor->Size.y/2;
                objbottom = dor->Position.y + dor->Size.y/2;
            }


            bool overlapX = playerLeft < objright && playerRight > objleft;
            bool overlapY = playerTop < objbottom && playerBottom > objtop;

            if (overlapX && overlapY) return true;
        }
    }
    return false;
}

void Player::PlayerEnemyCollision(Player *player, float time) {
    PlayScene* scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
    if (!scene) return;

    float halfSize_x = abs(player->Size.x) / 2;
    float playerLeft = player->Position.x - halfSize_x + tolerance;
    float playerRight = player->Position.x + halfSize_x - tolerance;
    float playerTop = player->Position.y + tolerance;
    float playerBottom = player->Position.y + player->Size.y - tolerance;

    for (auto &it : scene->EnemyGroup->GetObjects()) {
        Enemy* enemy = dynamic_cast<Enemy*>(it);
        if (!enemy) {
            Tint = al_map_rgb(255,255,255);
            continue;
        }

        if (enemy->getHp() <= 0) {
            enemy = nullptr;
            continue;
        }

        int half_enemy = abs(enemy->Size.x) / 2;

        float enemy_Left = enemy->Position.x - half_enemy;
        float enemy_Right = enemy->Position.x + half_enemy;
        float enemy_Top = enemy->Position.y;
        float enemy_Bottom = enemy->Position.y + enemy->Size.y;

        bool overlapX = playerLeft < enemy_Right && playerRight > enemy_Left;
        bool overlapY = playerTop < enemy_Bottom && playerBottom > enemy_Top;

        if (overlapX && overlapY && !player->isKnockedback) {
            float direction = (abs(enemy->Position.x) > abs(player->Position.x)) ? -1 : 1;
            player->knockbackVelocityX = direction * PlayScene::BlockSize * 1;
            player->Hit(enemy->getDamage());

            player->knockbackTimer = player->maxKnockbackTime;
            player->isKnockedback = true;
            player->Tint = al_map_rgb(255, 0, 0); // Explicitly set tint here
        }
        else if (player->isKnockedback) {
            float newX = player->Position.x + player->knockbackVelocityX * time;
            if (!player->IsCollision(newX, player->Position.y)) {
                player->Position.x = newX;
            } else {
                player->knockbackVelocityX *= -0.3f;
            }
            player->knockbackTimer -= time;
            if (player->knockbackTimer <= 0 || scene->EnemyGroup->GetObjects().empty()) {
                player->isKnockedback = false;
                player->knockbackVelocityX = 0;
                player->Tint = al_map_rgb(255, 255, 255);
            }
        }
    }

}

void Player::PlayerBulletCollision(Player *player, float time) {
    if (player->isBulletKnockback) {
        player->bulletKnockbackTimer -= time;
        if (player->bulletKnockbackTimer <= 0) {
            player->isBulletKnockback = false;
            player->bulletKnockbackVelocityX = 0;
            player->Tint = al_map_rgb(255, 255, 255);
        }
    }
}


void Player::Hit(float damage) {
    this->hp -= damage;
}
void Player::remove() {
    auto *scene = getPlayScene();
    scene->PlayerGroup->RemoveObject(GetObjectIterator());
}


