//
// Created by Hsuan on 2024/4/10.
//

#include "StartScene.h"
#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/StartScene.h"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

// TODO HACKATHON-2 (1/3): You can imitate the 2 files: 'StartScene.hpp', 'StartScene.cpp' to implement your SettingsScene.
void StartScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    AddNewObject(new Engine::Label("Tower Defense", "pirulen.ttf", 120, halfW, halfH / 3 + 50, 10, 255, 255, 255, 0.5, 0.5));

    //AddNewObject(new Engine::Image("trophy.png", 60, h - 150, 85, 85));
    btn = new Engine::ImageButton("trophy.png", "play/floor.png", 60, h - 150, 85, 85);
    btn->SetOnClickCallback(std::bind(&StartScene::TrophyOnClick, this, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Scoreboard", "pirulen.ttf", 16, 60 + 43, h - 42, 220, 220, 220, 205, 0.5, 0.5));


    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 200, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Play", "pirulen.ttf", 48, halfW, halfH / 2 + 250, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 *2.5, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::SettingsOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Settings", "pirulen.ttf", 48, halfW, halfH / 2 *2.5 + 50, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 *3, 400, 100);
    btn->SetOnClickCallback([]() {
        Engine::GameEngine::shouldQuit = true;
    });
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Quit Game", "pirulen.ttf", 48, halfW, halfH / 2 *3 + 50, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("play/target-invalid.png", "play/floor.png", w - 100, 0, 100, 100);
    btn->SetOnClickCallback([]() {
        Engine::GameEngine::shouldQuit = true;
    });
    AddNewControlObject(btn);
}
void StartScene::Terminate() {
    IScene::Terminate();
}
void StartScene::PlayOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void StartScene::SettingsOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("settings");
}
void StartScene::TrophyOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}