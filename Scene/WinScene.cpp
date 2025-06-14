#include <functional>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "WinScene.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "SelectProfileScene.h"
#include "StageSelectScene.hpp"

void WinScene::Initialize() {
    ticks = 0;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    PlayScene::Camera.x = 0;
    PlayScene::Camera.y = 0;
    AddNewObject(new Engine::Image("win/trophy.png", halfW, halfH, PlayScene::BlockSize*5.5, PlayScene::BlockSize*7, 0.5, 0.5));
    AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 29*PlayScene::BlockSize/60, halfW, halfH / 4, 255, 215,  0, 255, 0.5, 0.5));
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("start/button.png", "win/floor.png", halfW - 1.5*PlayScene::BlockSize, h-1.5*PlayScene::BlockSize, 3*PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&WinScene::ReturnOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Next", "pirulen.ttf", 2*PlayScene::BlockSize/5, halfW, h-PlayScene::BlockSize, 10, 255, 255, 255, 0.5, 0.5));
    bgmId = AudioHelper::PlayAudio("win.wav");
}
void WinScene::Terminate() {
    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}
void WinScene::Update(float deltaTime) {
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
        ticks = 100;
        bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}
void WinScene::ReturnOnClick(int stage) {
    if (SelectProfileScene::isSaved && !SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].isWin) {

        //UPDATE PROFILE --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        std::time_t now = std::time(nullptr);

        // Convert to local time
        std::tm tm{};

        #ifdef _WIN32
            localtime_s(&tm, &now);  // Windows
        #else
            localtime_r(&now, &tm);  // Linux / macOS
        #endif

        // Format date with slashes: YYYY/MM/DD
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y/%m/%d  %H:%M");
        std::string date_time = oss.str();

        SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Last_Played = date_time;
        SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Duration = std::to_string(std::stof(SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Duration) + PlayScene::total_time);
        SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].isWin = true;
        SelectProfileScene::WriteProfileData(nullptr);
        //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

        //SAVE TO LEADERBOARD ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        const int total_points = static_cast<int>(std::stof(SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Coins) * 3600.0f / std::stof(SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Duration));

        setScore(total_points);
        std::string Name = SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Name;
        std::string dur = SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Duration;

        std::string filename = "../Resource/scoreboard.txt";
        std::ofstream ofs(filename, std::ios_base::app);

        if (ofs.is_open()) {

            ofs
            << Name << "~"
            << std::to_string(score) << "~"
            << dur << std::endl;

            ofs.close();
        } else {
            std::cerr << "[ERROR] Could not open scoreboard.txt for writing!" << std::endl;
        }
        //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    }

    Engine::GameEngine::GetInstance().ChangeScene("credit");
}

void WinScene::setScore(int scr) {
    score = scr;
}
