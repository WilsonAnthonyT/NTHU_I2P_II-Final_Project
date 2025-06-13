#include "GasMachine.h"
#include "Gas.h"
#include "Engine/GameEngine.hpp"
#include "Scene/PlayScene.hpp"

PlayScene *GasMachine::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

GasMachine::GasMachine(std::string img, float x, float y) : Sprite(img, x, y) {
    active = false;
    Anchor = Engine::Point(0.5, 0);
    Size.x = PlayScene::BlockSize, Size.y = PlayScene::BlockSize;
    gasSpawnTimer = 0.0f;  // Initialize timer
    currentGasIndex = 0;   // Start from i = 0
}

void GasMachine::Update(float deltaTime) {
    auto scene = getPlayScene();
    if (!scene) return;

    // Check if player is colliding to activate the machine
    if (!active && IsCollision(Position.x, Position.y)) {
        active = true;
    }

    if (active) {
        gasSpawnTimer += deltaTime;  // Increment timer

        // Spawn gas every 1 second (until i reaches 19)
        if (gasSpawnTimer >= 1.0f && currentGasIndex < 20) {
            gasSpawnTimer = 0.0f;  // Reset timer

            // Calculate the Y position for the new gas
            float gasY = Position.y - currentGasIndex * PlayScene::BlockSize;
            
            // Spawn gas at (Position.x, gasY)
            scene->InteractiveBlockGroup->AddNewObject(new Gas("play/gas.png", Position.x, gasY));
            
            currentGasIndex++;  // Move to the next position
        }
    }
}

bool GasMachine::IsCollision(float x, float y) {
    PlayScene* scene = getPlayScene();
    if (!scene) return false;

    // Calculate hitbox based on object size
    float left = x - 3 * Size.x / 2;
    float right = x + 3 * Size.x / 2;

    for (auto& it : scene->PlayerGroup->GetObjects()) {
        Player* player = dynamic_cast<Player*>(it);
        if (!player || !player->Visible) continue;
        
        int half_P = abs(player->Size.x) / 2;
        float p_Left = player->Position.x - half_P;
        float p_Right = player->Position.x + half_P;

        bool overlapX = left < p_Right && right > p_Left;

        if (overlapX) {
            return true;
        }
    }

    return false;
}