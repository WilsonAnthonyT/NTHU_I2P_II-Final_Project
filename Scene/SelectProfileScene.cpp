//
// Created by MiHu on 6/11/2025.
//

#include "SelectProfileScene.h"

#include <fstream>
#include <allegro5/allegro_audio.h>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"

#include "StartScene.h"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void SelectProfileScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;
    std::vector<ProfileData> playerData; //(MAXprofile,{"+", "", "", ""});

    std::string summfile = "../Resource/player-data/data-summary.txt";
    std::ifstream ifs(summfile, std::ios_base::in);

    if (ifs.is_open()) {
        std::string str;
        int user_idx = 0;
        while (getline(ifs, str)){
            if (str[0] == '#' || str.empty()) {
                str.clear();
                continue;
            }

            std::stringstream ss(str);
            std::string name, made, last, dur;
            while (
            getline(ss, name, '~') &&
            getline(ss, made, '~') &&
            getline(ss, last, '~') &&
            getline(ss, dur)
            ) {
                playerData.push_back( {
                    name.substr(0,name.length()-1),
                    (made.substr(1, made.length()-1)),
                    last.substr(1, last.length()-1),
                    dur.substr(1, dur.length())
                });

                name.clear();
                made.clear();
                last.clear();
                dur.clear();
            }

            str.clear();
        }

        ifs.close();
    } else {
        std::cerr << "[ERROR] Could not open DATA SUMMARY for reading!" << std::endl;
    }

    for (int i = 1; i <= 3; i++) {
        btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW - PlayScene::BlockSize*3/2, i*PlayScene::BlockSize + (h-(7*PlayScene::BlockSize))/5 , PlayScene::BlockSize*3, PlayScene::BlockSize);
        btn->SetOnClickCallback(std::bind(&SelectProfileScene::PlayOnClick, this, i));
        AddNewControlObject(btn);
        AddNewObject(new Engine::Label(playerData[i-1].Name, "pirulen.ttf", PlayScene::BlockSize/3, halfW, i*PlayScene::BlockSize + (h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*0.5, 10, 255, 255, 255, 0.5, 0.5));
    }


    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW - PlayScene::BlockSize*3/2, 6*PlayScene::BlockSize + (h-(7*PlayScene::BlockSize))/5 , PlayScene::BlockSize*3, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SelectProfileScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", PlayScene::BlockSize/3, halfW, 6*PlayScene::BlockSize + (h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*0.5, 10, 255, 255, 255, 0.5, 0.5));

    //bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}


void SelectProfileScene::Terminate() {
    // AudioHelper::StopSample(bgmInstance);
    // bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void SelectProfileScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void SelectProfileScene::PlayOnClick(int stage) {
    ReadProfileData(stage);
}

void SelectProfileScene::ReadProfileData(int ID) {
    std::string filename = "../Resource/player-data/data" + std::to_string(ID) + ".txt";
    std::ifstream ifs(filename, std::ios_base::in);

    if (ifs.is_open()) {
        std::string line;
        int map_idx = -1;

        while (getline(ifs, line)) {
            if (line[0] == '#') {
                line.clear();
                continue;
            }

            if (map_idx == -1) {
                map_idx = stoi(line);
            }

            line.clear();
        }

        if (map_idx != -1) {
            PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
            scene->MapId = map_idx;
            Engine::GameEngine::GetInstance().ChangeScene("play");
        }

        ifs.close();
    } else {
        std::cerr << "[ERROR] Could not open PLAYER PROFILE DATA for reading!" << std::endl;
    }
}

