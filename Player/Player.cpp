#include "Player.h"

#include <iostream>
#include <allegro5/allegro_primitives.h>
#include "Engine/IScene.hpp"

#include "Engine/GameEngine.hpp"
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"

Player::Player(std::string img, float x, float y, float speed, float hp): Sprite(img,x,y),speed(speed), hp(hp),flipped(false){
    MaxHp = hp;
    TintTimer = 0.0f;
    verticalVelocity = 0.0f;
    isJumping = false;
    isFalling = false;
    goDown = false;
    // Calculate jump force for exactly 1.5 blocks (96 pixels) high
    jumpForce = sqrt(2 * PlayScene::Gravity * ((PlayScene::BlockSize)/2)*3); // 192 pixels/s
    Size = Engine::Point(PlayScene::BlockSize * 0.7, PlayScene::BlockSize * 0.7);
    Anchor = Engine::Point(0.5, 0);
    tolerance = 1.0 / 64.0 * PlayScene::BlockSize;
}

void Player::Update(float deltaTime) {

}

void Player::Draw() const {
    Sprite::Draw();
    if (Engine::IScene::DebugMode) {
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

    // Screen boundaries
    Engine::Point MapBound=PlayScene::GetClientSize();
    float ScreenBound = PlayScene::Camera.x + Engine::GameEngine::GetInstance().GetScreenWidth();

    if (playerLeft < 0 || playerRight > MapBound.x ||
        playerTop < 0 || playerBottom > MapBound.y) {
        return true;
    }

    if (playerLeft < PlayScene::Camera.x || playerRight > ScreenBound) {
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
                else if (scene->mapState[yTile][xTile] == PlayScene::TILE_WPLATFORM) {
                    // Only collide if player's bottom is touching the platform's top
                    float platformTop = yTile * PlayScene::BlockSize;
                    float platformBottom = platformTop + PlayScene::BlockSize;

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
    return false;
}

