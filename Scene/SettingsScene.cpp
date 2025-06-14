#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/SettingsScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void SettingsScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    float Blocksize = w/16;
    backgroundIMG = Engine::Resources::GetInstance().GetBitmap("start/settingbg.png");
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("start/button.png", "stage-select/floor.png", halfW-1.5*Blocksize, h-2.5*Blocksize, 3*Blocksize, Blocksize);
    btn->SetOnClickCallback(std::bind(&SettingsScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 2 * Blocksize/5, halfW, h-2*Blocksize, 10, 255, 255, 255, 0.5, 0.5));

    Slider *BGM, *SFX;
    BGM = new Slider(halfW - Blocksize*1.1, halfH - Blocksize/2 - Blocksize/60, 3*Blocksize, Blocksize/30);
    BGM->SetOnValueChangedCallback(std::bind(&SettingsScene::BGMSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(BGM);
    AddNewObject(new Engine::Label("BGM: ", "pirulen.ttf", Blocksize * 7/30,halfW - 2 * Blocksize, halfH - Blocksize/2, 255, 255, 255, 255, 0, 0.5));
    SFX = new Slider(halfW - Blocksize*1.1, halfH + Blocksize/2 - Blocksize/60, 3 *Blocksize, Blocksize/30);
    SFX->SetOnValueChangedCallback(std::bind(&SettingsScene::SFXSlideOnValueChanged, this, std::placeholders::_1));
    AddNewControlObject(SFX);
    AddNewObject(new Engine::Label("SFX: ", "pirulen.ttf", Blocksize * 7/30, halfW - 2 * Blocksize, halfH + Blocksize/2, 255, 255, 255, 255, 0, 0.5));
    AddNewObject(new Engine::Image("start/bryancon.png",w-3 *Blocksize,halfH+Blocksize,5.5*Blocksize,8*Blocksize,0.5,0.5));
    AddNewObject(new Engine::Image("start/arwencon.png",3*Blocksize,halfH+Blocksize,5.5*Blocksize,8*Blocksize,0.5,0.5));

    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    // bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
    if (!AudioHelper::sharedBGMInstance ||
    !al_get_sample_instance_playing(AudioHelper::sharedBGMInstance.get())) {
        AudioHelper::sharedBGMInstance = AudioHelper::PlaySample("Highscores_bgm.mp3", true, AudioHelper::BGMVolume);
    }

    BGM->SetValue(AudioHelper::BGMVolume);
    SFX->SetValue(AudioHelper::SFXVolume);
}
void SettingsScene::Terminate() {
    // AudioHelper::StopSample(bgmInstance);
    // bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void SettingsScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void SettingsScene::BGMSlideOnValueChanged(float value) {
    AudioHelper::ChangeSampleVolume(AudioHelper::sharedBGMInstance, value);
    AudioHelper::BGMVolume = value;
}
void SettingsScene::SFXSlideOnValueChanged(float value) {
    AudioHelper::SFXVolume = value;
}
