#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "StageSelectScene.hpp"

#include "StartScene.h"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void StageSelectScene::Initialize() {
    float w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    float halfW = w / 2;
    float halfH = h / 2;
    float Blocksize = w/16;
    Engine::ImageButton *btn;
    PlayScene::Camera.x = 0;
    PlayScene::Camera.y = 0;
    backgroundIMG = Engine::Resources::GetInstance().GetBitmap("play/shipbackground-2.png");

    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW-1.5*Blocksize, (h-7*(0.8*Blocksize))/8, 3*Blocksize, 0.8*Blocksize);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 1", "pirulen.ttf", 2*Blocksize/5, halfW, (h-7*(0.8*Blocksize))/8 + 0.4*Blocksize, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW-1.5*Blocksize, (0.8*Blocksize)+2*((h-7*(0.8*Blocksize))/8), 3*Blocksize, 0.8*Blocksize);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 2", "pirulen.ttf", 2*Blocksize/5, halfW, (0.8*Blocksize)+2*((h-7*(0.8*Blocksize))/8)+0.4*Blocksize, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW-1.5*Blocksize, 2*(0.8*Blocksize)+3*((h-7*(0.8*Blocksize))/8), 3*Blocksize, 0.8*Blocksize);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 3));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 3", "pirulen.ttf", 2*Blocksize/5, halfW, 2*(0.8*Blocksize)+3*((h-7*(0.8*Blocksize))/8)+0.4*Blocksize, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW-1.5*Blocksize, 3*(0.8*Blocksize)+4*((h-7*(0.8*Blocksize))/8), 3*Blocksize, 0.8*Blocksize);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 4));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 4", "pirulen.ttf", 2*Blocksize/5, halfW, 3*(0.8*Blocksize)+4*((h-7*(0.8*Blocksize))/8)+0.4*Blocksize, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW-1.5*Blocksize, 4*(0.8*Blocksize)+5*((h-7*(0.8*Blocksize))/8), 3*Blocksize, 0.8*Blocksize);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 5));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 5", "pirulen.ttf", 2*Blocksize/5, halfW, 4*(0.8*Blocksize)+5*((h-7*(0.8*Blocksize))/8)+0.4*Blocksize, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW-1.5*Blocksize, 5*(0.8*Blocksize)+6*((h-7*(0.8*Blocksize))/8), 3*Blocksize, 0.8*Blocksize);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::PlayOnClick, this, 6));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 6", "pirulen.ttf", 2*Blocksize/5, halfW, 5*(0.8*Blocksize)+6*((h-7*(0.8*Blocksize))/8)+0.4*Blocksize, 10, 255, 255, 255, 0.5, 0.5));

    //back button
    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW-1.5*Blocksize, 6*(0.8*Blocksize)+7*((h-(7*0.8*Blocksize))/8), 3*Blocksize, 0.8*Blocksize);
    btn->SetOnClickCallback(std::bind(&StageSelectScene::BackOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("back", "pirulen.ttf", 2*Blocksize/5, halfW, 6*(0.8*Blocksize)+7*((h-(7*0.8*Blocksize))/8)+0.4*Blocksize, 10, 255, 255, 255, 0.5, 0.5));

    // Slider *sliderBGM, *sliderSFX;
    // sliderBGM = new Slider(40 + halfW - 95, halfH - 50 - 2, 190, 4);
    // sliderBGM->SetOnValueChangedCallback(std::bind(&StageSelectScene::BGMSlideOnValueChanged, this, std::placeholders::_1));
    // AddNewControlObject(sliderBGM);
    // AddNewObject(new Engine::Label("BGM: ", "pirulen.ttf", 28, 40 + halfW - 60 - 95, halfH - 50, 255, 255, 255, 255, 0.5, 0.5));
    // sliderSFX = new Slider(40 + halfW - 95, halfH + 50 - 2, 190, 4);
    // sliderSFX->SetOnValueChangedCallback(std::bind(&StageSelectScene::SFXSlideOnValueChanged, this, std::placeholders::_1));
    // AddNewControlObject(sliderSFX);
    // AddNewObject(new Engine::Label("SFX: ", "pirulen.ttf", 28, 40 + halfW - 60 - 95, halfH + 50, 255, 255, 255, 255, 0.5, 0.5));
    // // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    // bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    // sliderBGM->SetValue(AudioHelper::BGMVolume);
    // sliderSFX->SetValue(AudioHelper::SFXVolume);

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    // bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    if (!AudioHelper::sharedBGMInstance ||
    !al_get_sample_instance_playing(AudioHelper::sharedBGMInstance.get())) {
        AudioHelper::sharedBGMInstance = AudioHelper::PlaySample("Highscores_bgm.mp3", true, AudioHelper::BGMVolume);
    }
}
void StageSelectScene::Terminate() {
    // AudioHelper::StopSample(bgmInstance);
    // bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void StageSelectScene::BackOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void StageSelectScene::PlayOnClick(int stage) {
    PlayScene *scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    scene->MapId = stage;
    Engine::GameEngine::GetInstance().ChangeScene("story");
}
void StageSelectScene::ScoreboardOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard-scene");
}
// void StageSelectScene::BGMSlideOnValueChanged(float value) {
//     AudioHelper::ChangeSampleVolume(bgmInstance, value);
//     AudioHelper::BGMVolume = value;
// }
// void StageSelectScene::SFXSlideOnValueChanged(float value) {
//     AudioHelper::SFXVolume = value;
// }
