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

bool Engine::IScene::DebugMode = false;
void StartScene::Initialize() {
    PlayScene::BlockSize = Engine::GameEngine::GetInstance().GetScreenWidth() / 16;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    PlayScene::MapWidth = w;
    PlayScene::MapHeight = h;
    int halfW = w / 2;
    int halfH = h / 2;
    PlayScene::Camera.x = 0;
    PlayScene::Camera.y = 0;
    backgroundIMG = Engine::Resources::GetInstance().GetBitmap("start/startbg.png");
    Engine::ImageButton *btn;

    AddNewObject(new Engine::Image("start/ship.png",w,PlayScene::BlockSize*-0.5,PlayScene::BlockSize*10,PlayScene::BlockSize*6,1,0));

    AddNewObject(new Engine::Label("Invasion", "pirulen.ttf", PlayScene::BlockSize, PlayScene::BlockSize, PlayScene::BlockSize*1.5, 10, 255, 255, 255, 0 , 0.5));
    AddNewObject(new Engine::Label("requiem", "pirulen.ttf", PlayScene::BlockSize,  halfW, PlayScene::BlockSize*2.5, 10, 255, 255, 255, 1, 0.5));

    //AddNewObject(new Engine::Image("trophy.png", 60, h - 150, 85, 85));
    btn = new Engine::ImageButton("trophy.png", "play/floor.png", PlayScene::BlockSize*0.6, h - PlayScene::BlockSize*1.5, PlayScene::BlockSize*0.85, PlayScene::BlockSize*0.85);
    btn->SetOnClickCallback(std::bind(&StartScene::TrophyOnClick, this, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Leaderboard", "pirulen.ttf", PlayScene::BlockSize*16/100, PlayScene::BlockSize * 1.03, h - PlayScene::BlockSize*0.42, 220, 220, 220, 205, 0.5, 0.5));


    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", PlayScene::BlockSize*3, 3*PlayScene::BlockSize + (h-(7*PlayScene::BlockSize))/5 , PlayScene::BlockSize*3, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&StartScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Play", "pirulen.ttf", PlayScene::BlockSize/3, halfW/2 + PlayScene::BlockSize*0.5, 3*PlayScene::BlockSize + (h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*0.5, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", PlayScene::BlockSize*3, 4*PlayScene::BlockSize + 2*(h-(7*PlayScene::BlockSize))/5 , PlayScene::BlockSize*3, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&StartScene::SettingsOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Settings", "pirulen.ttf", PlayScene::BlockSize/3, halfW/2 + PlayScene::BlockSize*0.5, 4*PlayScene::BlockSize + 2*(h-(7*PlayScene::BlockSize))/5+PlayScene::BlockSize*0.5, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", PlayScene::BlockSize*3, 5*PlayScene::BlockSize + 3*(h-(7*PlayScene::BlockSize))/5 , PlayScene::BlockSize*3, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&StartScene::DISPLAYonClick, this, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Credit", "pirulen.ttf", PlayScene::BlockSize/3, halfW/2 + PlayScene::BlockSize*0.5, 5*PlayScene::BlockSize + 3*(h-(7*PlayScene::BlockSize))/5+PlayScene::BlockSize*0.5, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", PlayScene::BlockSize*3.5, 6*PlayScene::BlockSize + 4*(h-(7*PlayScene::BlockSize))/5, PlayScene::BlockSize*2, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&StartScene::DISPLAYonClick, this, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Exit", "pirulen.ttf", PlayScene::BlockSize/3, halfW/2 + PlayScene::BlockSize*0.5, 6*PlayScene::BlockSize + 4*(h-(7*PlayScene::BlockSize))/5+PlayScene::BlockSize*0.5, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("play/target-invalid.png", "play/invalid-pressed.png", w - PlayScene::BlockSize, 0, PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&StartScene::DISPLAYonClick, this, 1));
    AddNewControlObject(btn);

    if (Engine::GameEngine::fullscreen) {
        btn = new Engine::ImageButton("play/minimise.png", "play/minimise-pressed.png", w - 2*PlayScene::BlockSize, 0, PlayScene::BlockSize, PlayScene::BlockSize);
        btn->SetOnClickCallback(std::bind(&StartScene::DISPLAYonClick, this, 2));
        AddNewControlObject(btn);
    }
    else {
        btn = new Engine::ImageButton("play/maximise.png", "play/maximise-pressed.png", w - 2*PlayScene::BlockSize, 0, PlayScene::BlockSize, PlayScene::BlockSize);
        btn->SetOnClickCallback(std::bind(&StartScene::DISPLAYonClick, this, 3));
        AddNewControlObject(btn);
    }
    AddNewObject(new Engine::Image("play/brywen.png",w,h,PlayScene::BlockSize*6.5,PlayScene::BlockSize*6.5,1,1));
}
void StartScene::Terminate() {
    IScene::Terminate();
}
void StartScene::PlayOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("profile-select");
}

void StartScene::CreditOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}

void StartScene::SettingsOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("settings");
}
void StartScene::TrophyOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}
void StartScene::DISPLAYonClick(int stage) {
    switch (stage) {
        case 0:
        case 1:
            Engine::GameEngine::shouldQuit = true;
            return;
        case 2:
            Engine::GameEngine::GetInstance().ToggleFullscreen();
            Engine::GameEngine::GetInstance().ChangeScene("start");
            return;
        case 3:
            Engine::GameEngine::GetInstance().ToggleFullscreen();
            Engine::GameEngine::GetInstance().ChangeScene("start");
            return;
        Default: return;
    }


}

