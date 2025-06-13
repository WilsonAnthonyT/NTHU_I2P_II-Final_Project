#include "Door.h"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Scene/PlayScene.hpp"

PlayScene *Door::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Door::Door(std::string img, float x, float y, DoorState ds, bool horizontal) : horizontal(horizontal), currState(ds), initState(ds), Engine::Sprite(img, x, y){
    Anchor = Engine::Point(0.5, 0.5);
    if (horizontal) {
        Size.x = PlayScene::BlockSize,Size.y=PlayScene::BlockSize*6/32;
    } else if (!horizontal) {
        Size.x = PlayScene::BlockSize*6/32,Size.y=PlayScene::BlockSize;
    }
    Bitmap = Engine::Resources::GetInstance().GetBitmap(img);
}

void Door::Update(float deltaTime) {
    auto scene = getPlayScene();

    Door* doorObj = dynamic_cast<Door*>(GetObjectIterator()->second);
    auto& sensorList = scene->SensorDoorAssignments[doorObj];

    bool ShouldBeChange = false;
    for (auto sense : sensorList){
        if (sense->active) {
            ShouldBeChange = false;
            break;
        }
        ShouldBeChange = true;
    }

    if (ShouldBeChange && currState != initState) {
        currState = initState;
        (currState == OPEN)?
            Tint = al_map_rgb(85, 55, 0) : Tint = al_map_rgb(255, 255, 255);
    }

    Sprite::Update(deltaTime);
}

