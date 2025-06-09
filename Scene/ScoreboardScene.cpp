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

#include "ScoreboardScene.hpp"

void ScoreboardScene::Initialize() {
    ifs.open("../Resource/scoreboard.txt", std::ios_base::in);
    if (ifs.is_open()) {
        std::string name, score, date_time;
        while (getline(ifs, name, '~') && getline(ifs, score, '~') && getline(ifs, date_time, '~')) {
            if (name != " ") {
                Data.push_back( {name.substr(0,name.length()-1), std::stoi(score.substr(1, score.length()-1)), date_time.substr(1, date_time.length()-1)} );
            }

            name.clear();
            score.clear();
            date_time.clear();
        }

        ifs.close();
    } else {
        std::cerr << "[ERROR] Could not open scoreboard.txt for reading!" << std::endl;
    }
    // Count pages needed to display data
    MAXPAGES = (static_cast<int>(Data.size()) + List_per_Page-1) / List_per_Page;
    sortData(sortType);

    int halfW = Engine::GameEngine::GetInstance().GetScreenSize().x / 2;
    int halfH = Engine::GameEngine::GetInstance().GetScreenSize().y / 2;

    AddNewObject(new Engine::Label("LEADERBOARD", "pirulen.ttf", 60, halfW, halfH / 4 - 20, 255, 255, 255, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Sort By:", "pirulen.ttf", 20, 40, 40, 240, 240, 240, 225, 0.0, 0.0));
    AddNewObject(new Engine::Label("Name", "pirulen.ttf", 25, halfW - 500 - 10, halfH / 4 + 70, 215, 215, 215, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Score", "pirulen.ttf", 25, halfW - 30, halfH / 4 + 70, 215, 215, 215, 255, 0.5, 0.5));
    AddNewObject(new Engine::Label("Date & Time", "pirulen.ttf", 25, halfW + 500 - 30, halfH / 4 + 70, 215, 215, 215, 255, 0.5, 0.5));

    Engine::ImageButton *btn;

    // Sort By
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/dirt.png", 40, 70, 165, 60);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::SortOnClick, this, 0));
    AddNewControlObject(btn);

    SortLabel = new Engine::Label(Stype, "pirulen.ttf", 24, 122, 98, 20, 20, 20, 255, 0.5, 0.5);
    AddNewObject(SortLabel);

    // Close Button
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200 - 5, halfH * 3 / 2 + 30, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::CloseOnClick, this, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Close", "pirulen.ttf", 48, halfW - 8, halfH * 3 / 2 + 80, 0, 0, 0, 255, 0.5, 0.5));

    // > Next Page
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200 - 5 + 500, halfH * 3 / 2 + 30, 100, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::PageButtonOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label(">", "pirulen.ttf", 88, halfW - 8 + 355, halfH * 3 / 2 + 78, 0, 0, 0, 255, 0.5, 0.5));

    // < Prev Page
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200 - 5 - 200, halfH * 3 / 2 + 30, 100, 100);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::PageButtonOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("<", "pirulen.ttf", 88, halfW - 8 - 350, halfH * 3 / 2 + 78, 0, 0, 0, 255, 0.5, 0.5));
}

void ScoreboardScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_RIGHT) {
        if (Page < MAXPAGES-1) Page += 1;
    } else if (keyCode == ALLEGRO_KEY_LEFT){
        if (Page > 0) Page -= 1;
    }
}

void ScoreboardScene::Draw() const {
    IScene::Draw();
    int halfW = Engine::GameEngine::GetInstance().GetScreenSize().x / 2;
    int halfH = Engine::GameEngine::GetInstance().GetScreenSize().y / 2;

    ALLEGRO_FONT* font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", 20).get();
    ALLEGRO_FONT* Numfont = Engine::Resources::GetInstance().GetFont("pirulen.ttf", 25).get();
    int point = List_per_Page * Page;
    for (int i = 0; i < List_per_Page; i++) {

        if (i + point < static_cast<int>(Data.size())) {
            al_draw_textf(
                Numfont, al_map_rgb(215, 215, 215),
                halfW - 660, halfH / 4 + 105 + i * Spacing,
                ALLEGRO_ALIGN_CENTER,
                "%d.", i + point + 1
            );

            al_draw_textf(
                font, al_map_rgb(150, 150, 150),
                halfW - 560, halfH / 4 + 105 + i * Spacing,
                ALLEGRO_ALIGN_LEFT,
                "%s", (Data[i + point].name).c_str()
            );

            al_draw_textf(
                font, al_map_rgb(150, 150, 150),
                halfW - 85, halfH / 4 + 105 + i * Spacing,
                ALLEGRO_ALIGN_LEFT,
                "%d", Data[i + point].scores
            );

            al_draw_textf(
                font, al_map_rgb(150, 150, 150),
                halfW + 365, halfH / 4 + 105 + i * Spacing,
                ALLEGRO_ALIGN_LEFT,
                "%s", (Data[i + point].date_n_time).c_str()
            );
        }
    }
}

void ScoreboardScene::sortData(int type){
    switch (type) {

        case 1: // by Name
            std::sort(Data.begin(), Data.end(), [](const auto& a, const auto& b)
            {
                size_t i = 0, j = 0;
                while (i < a.name.size() && j < b.name.size()) {
                    if (!std::isalpha(a.name[i])) { ++i; continue; }
                    if (!std::isalpha(b.name[j])) { ++j; continue; }

                    if (a.name[i] < b.name[j]) return true;
                    if (a.name[i] > b.name[j]) return false;

                    ++i;
                    ++j;
                }
            }
            );
        break;

        case 2: // by Time
            std::sort(Data.begin(), Data.end(), [](const auto& a, const auto& b)
            { return a.date_n_time > b.date_n_time; }
            );
        break;

        default: // by Score
            std::sort(Data.begin(), Data.end(), [](const auto& a, const auto& b)
            { return a.scores > b.scores; }
            );
        break;
    }
}

void ScoreboardScene::Terminate() {
    Data.clear();
    IScene::Terminate();
}

void ScoreboardScene::CloseOnClick(int stage) {
    // Change to select scene.
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void ScoreboardScene::PageButtonOnClick(int stage) {
    // Change to select scene.
    if (stage == 1) {
        if (Page < MAXPAGES-1) Page += 1;
    }
    else if (stage == 2) {
        if (Page > 0) Page -= 1;
    }
}

void ScoreboardScene::SortOnClick(int stage) {
    // sort the leaderboard
    sortType = (sortType + 1) % 3;
    switch (sortType) {
        case 0: Stype = "Score"; break;
        case 1: Stype = "Name"; break;
        case 2: Stype = "Current"; break;
        default: break;
    }
    SortLabel->Text = Stype;

    sortData(sortType);
}