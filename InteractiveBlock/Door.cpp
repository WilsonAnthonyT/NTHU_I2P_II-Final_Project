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
        for (int i = 1; i <= 2; i++) {
            ActiveAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/dor1-" + std::to_string(i) + ".png"));
        }
    } else if (!horizontal) {
        Size.x = PlayScene::BlockSize*6/32,Size.y=PlayScene::BlockSize;
        for (int i = 1; i <= 2; i++) {
            ActiveAnimation.push_back(Engine::Resources::GetInstance().GetBitmap("animation/dor-" + std::to_string(i) + ".png"));
        }
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
    }

    UpdateAnimation(deltaTime);
    Sprite::Update(deltaTime);
}

void Door::UpdateAnimation(float deltaTime) {
    if (currState == CLOSE) {
        animationTime += deltaTime;

        std::vector<std::shared_ptr<ALLEGRO_BITMAP>>* currentAnimation = nullptr;
        currentAnimation = &ActiveAnimation;
        frameDuration = 0.15f;

        if (animationTime >= frameDuration) {
            animationTime = 0;
            currentFrame = (currentFrame + 1) % currentAnimation->size();
            this->bmp = (*currentAnimation)[currentFrame]; // .get() to access raw pointer
        }
    }
    else {
        if (horizontal) {
            bmp = Engine::Resources::GetInstance().GetBitmap("animation/dor1-inactive.png");
        }
        else if (!horizontal) {
            bmp = Engine::Resources::GetInstance().GetBitmap("animation/dor-inactive.png");
        }
    }
}

