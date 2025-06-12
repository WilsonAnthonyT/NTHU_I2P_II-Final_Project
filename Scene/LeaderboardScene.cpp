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

int LeaderboardScene::val = 0;

void LeaderboardScene::Initialize() {
    int halfW = Engine::GameEngine::GetInstance().GetScreenSize().x / 2;
    int halfH = Engine::GameEngine::GetInstance().GetScreenSize().y / 2;

    AddNewObject(new Engine::Label("enter your name", "pirulen.ttf", 40, halfW, halfH / 4 + 90, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("press [enter] to save", "pirulen.ttf", 22, halfW-3, halfH / 4 + 340, 155, 255, 255, 125, 0.5, 0.5));

    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&LeaderboardScene::DontSaveOnClick, this, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Don't Save", "pirulen.ttf", 45, halfW - 2, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));
}

void LeaderboardScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    int length = strlen(al_keycode_to_name(keyCode));
    if (keyCode == ALLEGRO_KEY_ENTER && !Name.empty()) {
        tick = 120;
    } else if (keyCode == ALLEGRO_KEY_BACKSPACE && !Name.empty()){
        Name.pop_back();
    } else if (length == 1 && Name.length() <= maxChar) {
        Name += al_keycode_to_name(keyCode);
    } else if (keyCode == ALLEGRO_KEY_SPACE && Name.length() <= maxChar) {
        Name += " ";
    } else if (keyCode == ALLEGRO_KEY_MINUS && Name.length() <= maxChar) {
        Name += "-";
    }
}

void LeaderboardScene::Update(float deltaTime) {
    if (tick > 1) {
        tick--;
    }
    // else if (tick == 1) {
    //     tick = 0;
    //     setScore();
    //     // Get current time
    //     std::time_t now = std::time(nullptr);
    //
    //     // Convert to local time
    //     std::tm tm;
    //     #ifdef _WIN32
    //             localtime_s(&tm, &now);  // Windows
    //     #else
    //             localtime_r(&now, &tm);  // Linux / macOS
    //     #endif
    //
    //     // Format date with slashes: YYYY/MM/DD
    //     std::ostringstream oss;
    //     oss << std::put_time(&tm, "%Y/%m/%d  %H:%M");
    //     dateNtime = oss.str();
    //
    //     ofs.open("../Resource/scoreboard.txt", std::ios_base::app);
    //     if (ofs.is_open()) {
    //         std::cout << "Successfully saved name: " << Name << std::endl;
    //         ofs
    //         << Name << " ~ "
    //         << score << " ~ "
    //         << dateNtime << " ~"
    //         << std::endl;
    //
    //         ofs.close();
    //     } else {
    //         std::cerr << "[ERROR] Could not open scoreboard.txt for writing!" << std::endl;
    //     }
    //
    //     Engine::GameEngine::GetInstance().ChangeScene("start");
    // }

    else if (tick == 1) {
        tick = 0;

        // Get current time
        std::time_t now = std::time(nullptr);

        // Convert to local time
        std::tm tm;
        #ifdef _WIN32
                localtime_s(&tm, &now);  // Windows
        #else
                localtime_r(&now, &tm);  // Linux / macOS
        #endif

        // Format date with slashes: YYYY/MM/DD
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y/%m/%d  %H:%M");
        dateNtime = oss.str();

        auto& it = SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1];
        it.Name = Name;
        it.Created = dateNtime;
        it.Last_Played = "-";
        it.Duration = "0";
        it.Stage = "1";

        Name.clear();
        dateNtime.clear();

        std::string filename = "../Resource/player-data/data-summary.txt";
        std::ofstream ofs(filename, std::ios_base::out);

        if (ofs.is_open()) {
            ofs
            << "# everything related to the profile will be saved here" << std::endl
            << "# Name ~ date created ~ last played ~ Screen time ~ last stage" << std::endl
            << "# MAX data 3" << std::endl
            << std::endl;


            for (auto& it : SelectProfileScene::playerData) {
                ofs
                << it.Name << "~"
                << it.Created << "~"
                << it.Last_Played << "~"
                << it.Duration << "~"
                << it.Stage << std::endl;
            }

            for (auto& it : SelectProfileScene::playerData) {
                std::cout
                << it.Name << "."
                << it.Created << "."
                << it.Last_Played << "."
                << it.Duration << "."
                << it.Stage << std::endl;
            }


            ofs.close();
            auto scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
            scene->MapId = 1;
            Engine::GameEngine::GetInstance().ChangeScene("story");
        }
        else {
            std::cerr << "[ERROR] Could not open DATA SUMMARY for INITIAL WRITING!" << std::endl;
        }
    }
}

void LeaderboardScene::Draw() const {
    IScene::Draw();
    int halfW = Engine::GameEngine::GetInstance().GetScreenSize().x / 2;
    int halfH = Engine::GameEngine::GetInstance().GetScreenSize().y / 2;

    // Draw white rectangle
    al_draw_filled_rectangle(halfW - 450, halfH - 40 - 65, halfW + 450, halfH + 40 - 60,
        al_map_rgba(255, 255, 255, 255)
    );

    ALLEGRO_FONT* font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", 35).get();
    al_draw_textf(
        font, al_map_rgb(76, 64, 45),
        halfW, halfH - 85,
        ALLEGRO_ALIGN_CENTER,
        "%s" ,Name.c_str()
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
    // Change to select scene.
    Engine::GameEngine::GetInstance().ChangeScene("start");
}