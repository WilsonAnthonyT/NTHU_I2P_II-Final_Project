#include <iostream>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>

#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"

#include <allegro5/allegro_primitives.h>

#include "LeaderboardScene.hpp"

#include "PlayScene.hpp"
#include "SelectProfileScene.h"

bool SelectProfileScene::isSaved = false;

void LeaderboardScene::Initialize() {
    float W = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float H = Engine::GameEngine::GetInstance().GetScreenSize().y;
    float halfW = Engine::GameEngine::GetInstance().GetScreenSize().x / 2;
    float halfH = Engine::GameEngine::GetInstance().GetScreenSize().y / 2;
    float Blocksize = W/16;
    backgroundIMG = Engine::Resources::GetInstance().GetBitmap("play/shipbackground-2.png");
    AddNewObject(new Engine::Label("enter your name", "pirulen.ttf", Blocksize/3, halfW, Blocksize*3, 255, 255, 255, 255, 0.5,1));
    AddNewObject(new Engine::Label("press [enter] to save", "pirulen.ttf", Blocksize/4, halfW-3, 5.65*Blocksize, 105, 255, 255, 185, 0.5, 0.5));

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW - 2*Blocksize, H-3*Blocksize, 4*Blocksize, Blocksize);
    btn->SetOnClickCallback(std::bind(&LeaderboardScene::DontSaveOnClick, this, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Don't Save", "pirulen.ttf", 45, halfW, H-2.5*Blocksize, 10, 255, 255, 255, 0.5, 0.5));

    if (!AudioHelper::sharedBGMInstance ||
    !al_get_sample_instance_playing(AudioHelper::sharedBGMInstance.get())) {
        AudioHelper::sharedBGMInstance = AudioHelper::PlaySample("Highscores_bgm.mp3", true, AudioHelper::BGMVolume);
    }

    enterPressed = false;
}

void LeaderboardScene::OnKeyDown(int keyCode) {
    if (enterPressed) return;

    IScene::OnKeyDown(keyCode);
    int length = strlen(al_keycode_to_name(keyCode));
    if (keyCode == ALLEGRO_KEY_ENTER && !Name.empty()) {
        tick = 120;
        enterPressed = true;
        // Get current time
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
        dateNtime = oss.str();

        auto& player_data = SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1];
        player_data.Name = Name;
        player_data.Created = dateNtime;
        player_data.Last_Played = "-";
        player_data.Duration = "0";
        player_data.Coins = "0";
        player_data.Stage = "1";
        player_data.isWin = false;

        Name.clear();
        dateNtime.clear();

        std::string filename = "../Resource/player-data/data-summary.txt";
        std::ofstream ofs(filename, std::ios_base::out);

        if (ofs.is_open()) {
            ofs
            << "# everything related to the profile will be saved here" << std::endl
            << "# Name ~ date created ~ last played ~ Screen time ~ last stage" << std::endl
            << "# MAX data 4" << std::endl
            << std::endl;


            for (auto& it : SelectProfileScene::playerData) {
                std::string tmp = (it.isWin)? "1" : "0";
                ofs
                << it.Name << "~"
                << it.Created << "~"
                << it.Last_Played << "~"
                << it.Duration << "~"
                << it.Coins << "~"
                << it.Stage << "~"
                << tmp << std::endl;
            }

            ofs.close();

            SelectProfileScene::isSaved = true;
        }
        else {
            std::cerr << "[ERROR] Could not open DATA SUMMARY for INITIAL WRITING!" << std::endl;
        }
    } else if (keyCode == ALLEGRO_KEY_BACKSPACE && !Name.empty()){
        Name.pop_back();
    } else if (length == 1 && Name.length() < maxChar) {
        Name += al_keycode_to_name(keyCode);
    } else if (keyCode == ALLEGRO_KEY_SPACE && Name.length() < maxChar) {
        Name += " ";
    } else if (keyCode == ALLEGRO_KEY_MINUS && Name.length() < maxChar) {
        Name += "-";
    }
}

void LeaderboardScene::Update(float deltaTime) {
    if (tick > 1) {
        tick--;
    }

    else if (tick == 1) {
        tick = 0;

        auto scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
        scene->MapId = 1;
        Engine::GameEngine::GetInstance().ChangeScene("story");
    }
}

void LeaderboardScene::Draw() const {
    IScene::Draw();
    float W = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float H = Engine::GameEngine::GetInstance().GetScreenSize().y;
    float halfW = Engine::GameEngine::GetInstance().GetScreenSize().x / 2;
    float halfH = Engine::GameEngine::GetInstance().GetScreenSize().y / 2;
    float Blocksize = W / 16;

    // Draw white rectangle
    al_draw_filled_rectangle(halfW - 3.75 * Blocksize, halfH - Blocksize, halfW + 3.75 * Blocksize, halfH,
                             al_map_rgba(255, 255, 255, 255)
    );

    ALLEGRO_FONT *font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", Blocksize / 2).get();
    al_draw_textf(
        font, al_map_rgb(76, 64, 45),
        halfW, 4.25 * Blocksize,
        ALLEGRO_ALIGN_CENTER,
        "%s", Name.c_str()
    );

    if (tick > 0) {
        //al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_filled_rectangle(0, 0, Engine::GameEngine::GetInstance().GetScreenSize().x, Engine::GameEngine::GetInstance().GetScreenSize().y, al_map_rgba(0, 0, 0, 195));
        ALLEGRO_FONT* font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", 72).get();
        al_draw_text(font, al_map_rgb(255, 255, 255), halfW - 5, halfH - 90, ALLEGRO_ALIGN_CENTER, "saved successfully!");
    }
}

void LeaderboardScene::Terminate() {
    IScene::Terminate();
}

void LeaderboardScene::DontSaveOnClick(int stage) {
    Name.clear();

    SelectProfileScene::isSaved = false;
    auto scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = 1;
    Engine::GameEngine::GetInstance().ChangeScene("story");
}