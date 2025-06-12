//
// Created by MiHu on 6/11/2025.
//

#include "SelectProfileScene.h"

#include <fstream>
#include <allegro5/allegro_audio.h>
#include <functional>
#include <iostream>
#include <memory>
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

int SelectProfileScene::profileID = 0;

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
            std::string name, made, last, dur, level;
            while (
            getline(ss, name, '~') &&
            getline(ss, made, '~') &&
            getline(ss, last, '~') &&
            getline(ss, dur, '~') &&
            getline(ss, level)
            ) {
                playerData.push_back( {
                    name.substr(0,name.length()-1),
                    (made.substr(1, made.length()-1)),
                    last.substr(1, last.length()-1),
                    dur.substr(1, dur.length()-1),
                    level.substr(1, level.length()),
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
        btn->SetOnClickCallback(std::bind(&SelectProfileScene::PlayOnClick, this, stoi(playerData[i-1].Stage)));
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
void SelectProfileScene::PlayOnClick(int ID) {
    profileID = ID;
    PlayScene *scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = ID;
    Engine::GameEngine::GetInstance().ChangeScene("play");
}

SelectProfileScene::textData* SelectProfileScene::ReadProfileData(const int readID) {

    std::string filename = "../Resource/player-data/data" + std::to_string(readID) + ".txt";
    std::ifstream ifs(filename, std::ios_base::in);

    if (ifs.is_open()) {
        std::string line;
        auto *data = new textData();

        while (getline(ifs, line)) {
            if (line[0] == '#' || line.empty()) {
                line.clear();
                continue;
            }

            std::stringstream ss(line);
            std::string str;

            std::getline(ss, str,',');
            data->HP_1 = stoi(str);

            std::getline(ss, str);
            data->HP_2 = stoi(str);

            line.clear();
        }

        ifs.close();
        return data;
    }

    std::cerr << "[ERROR] Could not open PLAYER PROFILE DATA for READING!" << std::endl;
    return nullptr;
}

void SelectProfileScene::WriteProfileData(const int saveID, textData* allplayers) {
    std::string filename = "../Resource/player-data/data" + std::to_string(saveID) + ".txt";
    std::fstream ofs(filename, std::ios_base::out);

    if (ofs.is_open()) {


        ofs.close();
    } else {
        std::cerr << "[ERROR] Could not open PLAYER PROFILE DATA for WRITING!" << std::endl;
    }
}
