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
        std::string name, scors, dur;
        while (getline(ifs, name, '~') && getline(ifs, scors, '~') && getline(ifs, dur)) {
            if (name.empty() || name[0] == '#') continue;

            //float sc = (scores[0] == '0')? 0.00f : std::stof(scores);
            //float dr = (scores[0] == '0')? 0.00f : std::stof(dur);
            std::cout << "NAMA: " << name << ", SKOR:" << std::stoi(scors) << ", DURR:" << std::stof(dur) << std::endl;
            Data.push_back( {name, std::stoi(scors), std::stof(dur)} );

            name.clear();
            scors.clear();
            dur.clear();
        }

        ifs.close();
    } else {
        std::cerr << "[ERROR] Could not open scoreboard.txt for reading!" << std::endl;
    }
    // Count pages needed to display data
    MAXPAGES = (static_cast<int>(Data.size()) + List_per_Page-1) / List_per_Page;
    sortData(sortType);
    int W = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int H = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = Engine::GameEngine::GetInstance().GetScreenSize().x / 2;
    int halfH = Engine::GameEngine::GetInstance().GetScreenSize().y / 2;
    backgroundIMG = Engine::Resources::GetInstance().GetBitmap("scoreboard/sboardbg.png");
    AddNewObject(new Engine::Label("LEADERBOARD", "pirulen.ttf", PlayScene::BlockSize/2, halfW, PlayScene::BlockSize*1, 255, 255, 255, 255, 0.5, 0));
    AddNewObject(new Engine::Label("Sort By:", "pirulen.ttf", PlayScene::BlockSize/6,PlayScene::BlockSize/3 , PlayScene::BlockSize/3, 240, 240, 240, 225, 0.0, 0.0));
    AddNewObject(new Engine::Label("Name", "pirulen.ttf", PlayScene::BlockSize*5/24, PlayScene::BlockSize*4, PlayScene::BlockSize*2, 215, 215, 215, 255, 0, 1));
    AddNewObject(new Engine::Label("Score", "pirulen.ttf", PlayScene::BlockSize*5/24, halfW, PlayScene::BlockSize*2, 215, 215, 215, 255, 0.5,1));
    AddNewObject(new Engine::Label("Duration", "pirulen.ttf", PlayScene::BlockSize*5/24, halfW + PlayScene::BlockSize*3, PlayScene::BlockSize*2, 215, 215, 215, 255, 0, 1));

    Engine::ImageButton *btn;

    // Sort By
    btn = new Engine::ImageButton("start/button.png", "start/button.png", PlayScene::BlockSize/3, 7*PlayScene::BlockSize/12, 11*PlayScene::BlockSize/8, PlayScene::BlockSize/2);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::SortOnClick, this, 0));
    AddNewControlObject(btn);

    SortLabel = new Engine::Label(Stype, "pirulen.ttf", PlayScene::BlockSize/5, PlayScene::BlockSize/3 + 11*PlayScene::BlockSize/16, 7*PlayScene::BlockSize/12 + PlayScene::BlockSize/4, 10, 255, 255, 255, 0.5, 0.5);
    AddNewObject(SortLabel);

    // Close Button
    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW - 1.5*PlayScene::BlockSize, H-2*PlayScene::BlockSize, 3*PlayScene::BlockSize,  PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::CloseOnClick, this, 0));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Close", "pirulen.ttf", PlayScene::BlockSize*2/5, halfW, H-1.5*PlayScene::BlockSize, 10, 255, 255, 255, 0.5, 0.5));

    // > Next Page
    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW + 2.6*PlayScene::BlockSize, H-1.9*PlayScene::BlockSize, 0.8*PlayScene::BlockSize, 0.8*PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::PageButtonOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label(">", "pirulen.ttf", PlayScene::BlockSize*22/30, halfW + 3*PlayScene::BlockSize, H-1.5*PlayScene::BlockSize, 10, 255, 255, 255, 0.5, 0.5));

    // < Prev Page
    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW - 3.4*PlayScene::BlockSize, H-1.9*PlayScene::BlockSize, 0.8*PlayScene::BlockSize, 0.8*PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&ScoreboardScene::PageButtonOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("<", "pirulen.ttf", PlayScene::BlockSize*22/30, halfW - 3*PlayScene::BlockSize, H-1.5*PlayScene::BlockSize, 10, 255, 255, 255, 0.5, 0.5));
    //AddNewObject(new Engine::Label("1ABC", "pirulen.ttf", PlayScene::BlockSize, 3*PlayScene::BlockSize, 1.5*PlayScene::BlockSize, 255, 255, 255, 255, 0, 0.5));
    AddNewObject(new Engine::Image("scoreboard/bryanmedal.png",W,halfH+PlayScene::BlockSize,PlayScene::BlockSize*3.5,PlayScene::BlockSize*5,1,0.5));
    AddNewObject(new Engine::Image("scoreboard/arwenmedal.png",0,halfH+PlayScene::BlockSize,PlayScene::BlockSize*3.5,PlayScene::BlockSize*5,0,0.5));
    Spacing = ((H-(4*PlayScene::BlockSize))-(10*PlayScene::BlockSize/6))/11;
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

    ALLEGRO_FONT* font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", PlayScene::BlockSize/6).get();
    ALLEGRO_FONT* Numfont = Engine::Resources::GetInstance().GetFont("pirulen.ttf", PlayScene::BlockSize*5/24).get();
    int point = List_per_Page * Page;
    for (int i = 0; i < List_per_Page; i++) {

        if (i + point < static_cast<int>(Data.size())) {
            al_draw_textf(
                Numfont, al_map_rgb(255, 255, 255),
                PlayScene::BlockSize*3.5, 2*PlayScene::BlockSize + i * PlayScene::BlockSize/6 - i*PlayScene::BlockSize/360 + (i+1) * Spacing,
                ALLEGRO_ALIGN_CENTRE,
                "%d.", i + point + 1
            );

            al_draw_textf(
                font, al_map_rgb(255, 255, 255),
                PlayScene::BlockSize*4, 2*PlayScene::BlockSize + i * PlayScene::BlockSize/6 + (i+1) * Spacing,
                ALLEGRO_ALIGN_LEFT,
                "%s", (Data[i + point].name).c_str()
            );

            al_draw_textf(
                font, al_map_rgb(255, 255, 255),
                halfW - PlayScene::BlockSize/2, 2*PlayScene::BlockSize + i * PlayScene::BlockSize/6 + (i+1) * Spacing,
                ALLEGRO_ALIGN_LEFT,
                "%d", Data[i + point].scores
            );

            al_draw_textf(
                font, al_map_rgb(255,255,255),
                halfW + PlayScene::BlockSize*3, 2*PlayScene::BlockSize + i * PlayScene::BlockSize/6 + (i+1) * Spacing,
                ALLEGRO_ALIGN_LEFT,
                "%.3f", (Data[i + point].duration)
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
            { return a.duration < b.duration; }
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
        case 2: Stype = "Duration"; break;
        default: break;
    }
    SortLabel->Text = Stype;

    sortData(sortType);
}