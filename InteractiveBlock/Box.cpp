#include "Box.h"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"

PlayScene *Box::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Box::Box(std::string img, float x, float y) : Engine::Sprite(img, x, y){
    Anchor = Engine::Point(0.5, 0);
    Size.x=PlayScene::BlockSize/2,Size.y=PlayScene::BlockSize/2;
    Bitmap = Engine::Resources::GetInstance().GetBitmap(img);
}

void Box::Update(float deltaTime) {
    // Apply gravity
    PlayScene* scene = getPlayScene();
    if (scene) {
        verticalVelocity += PlayScene::Gravity * deltaTime;
    }

    Push(Position.x,Position.y);
    // Apply friction to horizontal movement

    Engine::Point previousPosition = Position;

    // Update Y position
    float newY = Position.y + verticalVelocity * deltaTime;
    if (!IsCollision(Position.x, newY)) {
        Position.y = newY;
    } else {
        // Hit ground or ceiling
        if (verticalVelocity > 0) {
            // Landed on ground
            verticalVelocity = 0;
            // Find the exact ground position
            float testY = Position.y;
            while (!IsCollision(Position.x, testY + 1)) {
                testY += 1;
            }
            Position.y = testY;
        }
    }
    // Update X position
    float newX = Position.x + Velocity.x * deltaTime;
    if (IsCollision(newX, Position.y)) {
        Velocity.x=0;
    }

    Sprite::Update(deltaTime);
}

bool Box::IsCollision(float x, float y) {
    PlayScene* scene = getPlayScene();
    if (!scene) return true;

    // Calculate hitbox based on object size
    float left = x - Size.x/2;
    float right = x + Size.x/2;
    float top = y;
    float bottom = y + Size.y;

    // Check out-of-bounds
    Engine::Point mapSize = PlayScene::GetClientSize();
    if (left < 0 || right > mapSize.x || top < 0 || bottom > mapSize.y)
        return true;

    // Check collision with map tiles
    int tileLeft = static_cast<int>(left / PlayScene::BlockSize);
    int tileRight = static_cast<int>(right / PlayScene::BlockSize);
    int tileTop = static_cast<int>(top / PlayScene::BlockSize);
    int tileBottom = static_cast<int>(bottom / PlayScene::BlockSize);

    for (int yTile = tileTop; yTile <= tileBottom; ++yTile) {
        for (int xTile = tileLeft; xTile <= tileRight; ++xTile) {
            if (xTile >= 0 && xTile < scene->MapWidth &&
                yTile >= 0 && yTile < scene->MapHeight) {
                int tileType = scene->mapState[yTile][xTile];
                if (tileType == PlayScene::TILE_DIRT) {
                    // Calculate tile boundaries
                    float tileLeft = xTile * PlayScene::BlockSize;
                    float tileRight = (xTile + 1) * PlayScene::BlockSize;
                    float tileTop = yTile * PlayScene::BlockSize;
                    float tileBottom = (yTile + 1) * PlayScene::BlockSize;

                    // Check for actual overlap
                    if (right > tileLeft && left < tileRight &&
                        bottom > tileTop && top < tileBottom) {
                        return true;
                    }
                }
            }
        }
    }

    for (auto& it : scene->PlayerGroup->GetObjects()) {
        Player* player = dynamic_cast<Player*>(it);
        // Skip if enemy is not visible, is the healer itself, or invalid
        if (!player || !player->Visible) continue;
        int half_P = abs(player->Size.x) / 2;

        float p_Left = player->Position.x - half_P;
        float p_Right = player->Position.x + half_P;
        float p_Top = player->Position.y;
        float p_Bottom = player->Position.y + player->Size.y;

        bool overlapX = left + PlayScene::BlockSize/8 < p_Right && right - PlayScene::BlockSize/8 > p_Left;
        bool overlapY = top < p_Bottom && bottom > p_Top;

        if (overlapX && overlapY) {
            return true;
        }
    }

    for (auto& it : scene->InteractiveBlockGroup->GetObjects()) {
        Box* friends = dynamic_cast<Box*>(it);
        // Skip if enemy is not visible, is the healer itself, or invalid
        if (!friends || !friends->Visible || friends == this) continue;
        int half_P2 = abs(friends->Size.x) / 2;

        float p2_Left = friends->Position.x - half_P2;
        float p2_Right = friends->Position.x + half_P2;
        float p2_Top = friends->Position.y;
        float p2_Bottom = friends->Position.y + friends->Size.y;

        bool overlapX = left < p2_Right && right > p2_Left;
        bool overlapY = top < p2_Bottom && bottom > p2_Top;

        if (overlapX && overlapY) {
            return true;
        }
    }

    return false;
}

bool Box::Push(float x, float y) {
    PlayScene* scene = getPlayScene();
    if (!scene) return true;

    // Calculate hitbox based on object size
    float left = x - Size.x/2;
    float right = x + Size.x/2;
    float top = y;
    float bottom = y + Size.y;
    for (auto& it : scene->PlayerGroup->GetObjects()) {
        Player* player = dynamic_cast<Player*>(it);
        // Skip if enemy is not visible, is the healer itself, or invalid
        if (!player || !player->Visible) continue;
        int half_P = abs(player->Size.x) / 2;

        float p_Left = player->Position.x - half_P;
        float p_Right = player->Position.x + half_P;
        float p_Top = player->Position.y;
        float p_Bottom = player->Position.y + player->Size.y;

        bool overlapX = left < p_Right && right > p_Left;
        bool overlapY = top < p_Bottom && bottom > p_Top;

        if (overlapX && overlapY) {
            if (player->Position.x < Position.x)Velocity.x = player->speed/2;
            if (player->Position.x > Position.x)Velocity.x = -player->speed/2;
            return true;
        }
    }
    Velocity.x=0;
    return false;
}