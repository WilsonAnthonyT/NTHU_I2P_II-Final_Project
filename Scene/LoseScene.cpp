#include <functional>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "LoseScene.hpp"

#include <iomanip>

#include "PlayScene.hpp"
#include "SelectProfileScene.h"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

void LoseScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    AddNewObject(new Engine::Image("lose/cry.png", halfW, halfH, PlayScene::BlockSize*6, PlayScene::BlockSize*6, 0.5, 0.5));
    AddNewObject(new Engine::Label("You Lose :(", "pirulen.ttf", 2*PlayScene::BlockSize/5, halfW, halfH / 4, 255, 0, 0, 255, 0.5, 0.5));
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("start/button.png", "win/floor.png", halfW - 1.5*PlayScene::BlockSize, h - 1.5*PlayScene::BlockSize, 3*PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&LoseScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 2*PlayScene::BlockSize/5, halfW, h-PlayScene::BlockSize, 10,255, 255, 255, 0.5, 0.5));
    bgmInstance = AudioHelper::PlaySample("losing.mp3", false, AudioHelper::BGMVolume, PlayScene::DangerTime);
}
void LoseScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void LoseScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
