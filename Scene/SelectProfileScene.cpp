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

std::vector<SelectProfileScene::ProfileData> SelectProfileScene::playerData;
int SelectProfileScene::profileID = 0;

void SelectProfileScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    playerData.clear();
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
            std::string name, made, last, dur, cc, level;
            while (
            getline(ss, name, '~') &&
            getline(ss, made, '~') &&
            getline(ss, last, '~') &&
            getline(ss, dur, '~') &&
            getline(ss, cc, '~') &&
            getline(ss, level)
            ) {
                playerData.push_back( {
                    name,
                    made,
                    last,
                    dur,
                    cc,
                    level
                });

                name.clear();
                made.clear();
                last.clear();
                dur.clear();
                cc.clear();
                level.clear();
            }

            str.clear();
        }

        ifs.close();
    } else {
        std::cerr << "[ERROR] Could not open DATA SUMMARY for reading!" << std::endl;
    }

    while (playerData.size() < MAXprofile) {
        playerData.push_back( {
            "+",
            "-",
            "-",
            "-",
            "0",
            "0"
        });
    }

    //profile 1
    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW - PlayScene::BlockSize*(3+4/2), 1.5*PlayScene::BlockSize + (-0.5)*(h-(7*PlayScene::BlockSize))/5 , PlayScene::BlockSize*4, PlayScene::BlockSize*2);
    btn->SetOnClickCallback(std::bind(&SelectProfileScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label(playerData[1-1].Name, "pirulen.ttf", PlayScene::BlockSize/3, halfW - PlayScene::BlockSize*(3+4/2) + PlayScene::BlockSize*(4/2), 1.5*PlayScene::BlockSize + (-0.5)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*0.5, 10, 255, 255, 255, 0.5, 0.5));

    AddNewObject(new Engine::Label("Created     : " + playerData[1-1].Created, "pirulen.ttf", PlayScene::BlockSize/8, halfW - PlayScene::BlockSize*(3+4/2) + PlayScene::BlockSize * 0.5, 1.5*PlayScene::BlockSize + (-0.5)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*(0.5 + 0.65), 10, 255, 255, 255, 0.0, 0.5));
    AddNewObject(new Engine::Label("Last played : " + playerData[1-1].Last_Played, "pirulen.ttf", PlayScene::BlockSize/8, halfW - PlayScene::BlockSize*(3+4/2) + PlayScene::BlockSize * 0.5, 1.5*PlayScene::BlockSize + (-0.5)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*(0.5 + 0.90), 10, 255, 255, 255, 0.0, 0.5));
    AddNewObject(new Engine::Label("Stage       : " + playerData[1-1].Stage, "pirulen.ttf", PlayScene::BlockSize/8, halfW - PlayScene::BlockSize*(3+4/2) + PlayScene::BlockSize * 0.5, 1.5*PlayScene::BlockSize + (-0.5)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*(0.5 + 1.15), 10, 255, 255, 255, 0.0, 0.5));

    //trash 1
    btn = new Engine::ImageButton("start/sampah.png", "start/sampah-pressed.png", halfW - PlayScene::BlockSize*(3+4/2) - PlayScene::BlockSize*(1 + 0.0625), 1.5*PlayScene::BlockSize + (-0.5)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize/2 , PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SelectProfileScene::RemoveOnClick, this, 1));
    AddNewControlObject(btn);


    //profile 2
    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW + PlayScene::BlockSize*(2/2), 1.5*PlayScene::BlockSize + (-0.5)*(h-(7*PlayScene::BlockSize))/5 , PlayScene::BlockSize*4, PlayScene::BlockSize*2);
    btn->SetOnClickCallback(std::bind(&SelectProfileScene::PlayOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label(playerData[2-1].Name, "pirulen.ttf", PlayScene::BlockSize/3, halfW + PlayScene::BlockSize*(2/2) + PlayScene::BlockSize*(4/2), 1.5*PlayScene::BlockSize + (-0.5)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*0.5, 10, 255, 255, 255, 0.5, 0.5));

    AddNewObject(new Engine::Label("Created     : " + playerData[2-1].Created, "pirulen.ttf", PlayScene::BlockSize/8, halfW + PlayScene::BlockSize*(2/2) + PlayScene::BlockSize * 0.5, 1.5*PlayScene::BlockSize + (-0.5)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*(0.5 + 0.65), 10, 255, 255, 255, 0.0, 0.5));
    AddNewObject(new Engine::Label("Last played : " + playerData[2-1].Last_Played, "pirulen.ttf", PlayScene::BlockSize/8, halfW + PlayScene::BlockSize*(2/2) + PlayScene::BlockSize * 0.5, 1.5*PlayScene::BlockSize + (-0.5)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*(0.5 + 0.90), 10, 255, 255, 255, 0.0, 0.5));
    AddNewObject(new Engine::Label("Stage       : " + playerData[2-1].Stage, "pirulen.ttf", PlayScene::BlockSize/8, halfW + PlayScene::BlockSize*(2/2) + PlayScene::BlockSize * 0.5, 1.5*PlayScene::BlockSize + (-0.5)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*(0.5 + 1.15), 10, 255, 255, 255, 0.0, 0.5));

    //trash 2
    btn = new Engine::ImageButton("start/sampah.png", "start/sampah-pressed.png", halfW + 4*PlayScene::BlockSize*(3/2) + PlayScene::BlockSize*(1 + 0.0625), 1.5*PlayScene::BlockSize + (-0.5)*(h-(7*PlayScene::BlockSize))/5  + PlayScene::BlockSize/2, PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SelectProfileScene::RemoveOnClick, this, 2));
    AddNewControlObject(btn);


    //profile 3
    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW - PlayScene::BlockSize*(3+4/2), 3.25*PlayScene::BlockSize + (1.25)*(h-(7*PlayScene::BlockSize))/5 , PlayScene::BlockSize*4, PlayScene::BlockSize*2);
    btn->SetOnClickCallback(std::bind(&SelectProfileScene::PlayOnClick, this, 3));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label(playerData[3-1].Name, "pirulen.ttf", PlayScene::BlockSize/3, halfW - PlayScene::BlockSize*(3+4/2) + PlayScene::BlockSize*(4/2), 3.25*PlayScene::BlockSize + (1.25)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*0.5, 10, 255, 255, 255, 0.5, 0.5));

    AddNewObject(new Engine::Label("Created     : " + playerData[3-1].Created, "pirulen.ttf", PlayScene::BlockSize/8, halfW - PlayScene::BlockSize*(3+4/2) + PlayScene::BlockSize * 0.5, 3.25*PlayScene::BlockSize + (1.25)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*(0.5 + 0.65), 10, 255, 255, 255, 0.0, 0.5));
    AddNewObject(new Engine::Label("Last played : " + playerData[3-1].Last_Played, "pirulen.ttf", PlayScene::BlockSize/8, halfW - PlayScene::BlockSize*(3+4/2) + PlayScene::BlockSize * 0.5, 3.25*PlayScene::BlockSize + (1.25)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*(0.5 + 0.90), 10, 255, 255, 255, 0.0, 0.5));
    AddNewObject(new Engine::Label("Stage       : " + playerData[3-1].Stage, "pirulen.ttf", PlayScene::BlockSize/8, halfW - PlayScene::BlockSize*(3+4/2) + PlayScene::BlockSize * 0.5, 3.25*PlayScene::BlockSize + (1.25)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*(0.5 + 1.15), 10, 255, 255, 255, 0.0, 0.5));

    //trash 3
    btn = new Engine::ImageButton("start/sampah.png", "start/sampah-pressed.png", halfW - PlayScene::BlockSize*(3+4/2) - PlayScene::BlockSize*(1 + 0.0625), 3.25*PlayScene::BlockSize + (1.25)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize/2 , PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SelectProfileScene::RemoveOnClick, this, 3));
    AddNewControlObject(btn);


    //profile 4
    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW + PlayScene::BlockSize*(2/2), 3.25*PlayScene::BlockSize + (1.25)*(h-(7*PlayScene::BlockSize))/5 , PlayScene::BlockSize*4, PlayScene::BlockSize*2);
    btn->SetOnClickCallback(std::bind(&SelectProfileScene::PlayOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label(playerData[4-1].Name, "pirulen.ttf", PlayScene::BlockSize/3, halfW + PlayScene::BlockSize*(2/2) + PlayScene::BlockSize*(4/2), 3.25*PlayScene::BlockSize + (1.25)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*0.5, 10, 255, 255, 255, 0.5, 0.5));

    AddNewObject(new Engine::Label("Created     : " + playerData[4-1].Created, "pirulen.ttf", PlayScene::BlockSize/8, halfW + PlayScene::BlockSize*(2/2) + PlayScene::BlockSize * 0.5, 3.25*PlayScene::BlockSize + (1.25)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*(0.5 + 0.65), 10, 255, 255, 255, 0.0, 0.5));
    AddNewObject(new Engine::Label("Last played : " + playerData[4-1].Last_Played, "pirulen.ttf", PlayScene::BlockSize/8, halfW + PlayScene::BlockSize*(2/2) + PlayScene::BlockSize * 0.5, 3.25*PlayScene::BlockSize + (1.25)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*(0.5 + 0.90), 10, 255, 255, 255, 0.0, 0.5));
    AddNewObject(new Engine::Label("Stage       : " + playerData[4-1].Stage, "pirulen.ttf", PlayScene::BlockSize/8, halfW + PlayScene::BlockSize*(2/2) + PlayScene::BlockSize * 0.5, 3.25*PlayScene::BlockSize + (1.25)*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*(0.5 + 1.15), 10, 255, 255, 255, 0.0, 0.5));

    //trash 4
    btn = new Engine::ImageButton("start/sampah.png", "start/sampah-pressed.png", halfW + 4*PlayScene::BlockSize*(3/2) + PlayScene::BlockSize*(1 + 0.0625), 3.25*PlayScene::BlockSize + (1.25)*(h-(7*PlayScene::BlockSize))/5  + PlayScene::BlockSize/2, PlayScene::BlockSize, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SelectProfileScene::RemoveOnClick, this, 4));
    AddNewControlObject(btn);


    //back button
    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW - PlayScene::BlockSize*3/2, 5.5*PlayScene::BlockSize + 3.5*(h-(7*PlayScene::BlockSize))/5 , PlayScene::BlockSize*3, PlayScene::BlockSize);
    btn->SetOnClickCallback(std::bind(&SelectProfileScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", PlayScene::BlockSize/3, halfW, 5.5*PlayScene::BlockSize + 3.5*(h-(7*PlayScene::BlockSize))/5 + PlayScene::BlockSize*0.5, 10, 255, 255, 255, 0.5, 0.5));

    //bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    if (!AudioHelper::sharedBGMInstance ||
    !al_get_sample_instance_playing(AudioHelper::sharedBGMInstance.get())) {
        AudioHelper::sharedBGMInstance = AudioHelper::PlaySample("Highscores_bgm.mp3", true, AudioHelper::BGMVolume);
    }
}


void SelectProfileScene::Terminate() {
    // AudioHelper::StopSample(bgmInstance);
    // bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void SelectProfileScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void SelectProfileScene::RemoveOnClick(int ID) {
    profileID = ID;

    playerData[ID-1].Name = "+";
    playerData[ID-1].Created = "-";
    playerData[ID-1].Last_Played = "-";
    playerData[ID-1].Duration = "-";
    playerData[ID-1].Coins = "0";
    playerData[ID-1].Stage = "0";

    WriteProfileData(nullptr);

    Engine::GameEngine::GetInstance().ChangeScene("profile-select");
}

void SelectProfileScene::PlayOnClick(int ID) {
    profileID = ID;

    if (playerData[ID-1].Name == "+") {
        Engine::GameEngine::GetInstance().ChangeScene("leaderboard");
    }
    else {
        PlayScene *scene = dynamic_cast<PlayScene*>(Engine::GameEngine::GetInstance().GetScene("play"));
        scene->MapId = stoi(playerData[ID-1].Stage);
        Engine::GameEngine::GetInstance().ChangeScene("story");
    }
}

// SelectProfileScene::textData* SelectProfileScene::ReadProfileData(const int readID) {
//
//     std::string filename = "../Resource/player-data/data" + std::to_string(readID) + ".txt";
//     std::ifstream ifs(filename, std::ios_base::in);
//
//     if (ifs.is_open()) {
//         std::string line;
//         auto *data = new textData();
//
//         while (getline(ifs, line)) {
//             if (line[0] == '#' || line.empty()) {
//                 line.clear();
//                 continue;
//             }
//
//             std::stringstream ss(line);
//             std::string str;
//
//             std::getline(ss, str,',');
//             data->HP_1 = stoi(str);
//
//             std::getline(ss, str);
//             data->HP_2 = stoi(str);
//
//             line.clear();
//         }
//
//         ifs.close();
//         return data;
//     }
//
//     std::cerr << "[ERROR] Could not open PLAYER PROFILE DATA for READING!" << std::endl;
//     return nullptr;
// }

void SelectProfileScene::WriteProfileData(textData* allplayers) {
    std::string filename = "../Resource/player-data/data-summary.txt";
    std::ofstream ofs(filename, std::ios_base::out);

    if (ofs.is_open()) {
        ofs
        << "# everything related to the profile will be saved here" << std::endl
        << "# Name ~ date created ~ last played ~ Screen time ~ Coins ~ last stage" << std::endl
        << "# MAX data 3" << std::endl
        << std::endl;

        if (allplayers) {
            playerData[profileID-1].Stage = std::to_string(allplayers->level);
            playerData[profileID-1].Coins = std::to_string(std::stoi(playerData[profileID-1].Coins) + allplayers->coin_counts);
        }

        for (auto& it : SelectProfileScene::playerData) {
            ofs
            << it.Name << "~"
            << it.Created << "~"
            << it.Last_Played << "~"
            << it.Duration << "~"
            << it.Coins << "~"
            << it.Stage << std::endl;

            // ofs << it.Name << "~"
            //     << it.Created << "~"
            //     << it.Last_Played << "~";
            //
            // // Set precision for the Duration value (0.00f)
            // ofs << std::fixed << std::setprecision(2) << std::stod(it.Duration) << "~";
            //
            // ofs << it.Coins << "~"
            //     << it.Stage << std::endl;
        }

        ofs.close();
    }
    else {
        std::cerr << "[ERROR] Could not open DATA SUMMARY for INITIAL WRITING!" << std::endl;
    }
}
