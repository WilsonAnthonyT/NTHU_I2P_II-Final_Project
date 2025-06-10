//
// Created by MiHu on 6/11/2025.
//

#include "CutScene.h"

#include <allegro5/allegro_primitives.h>

#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Label.hpp"

void CutScene::Initialize() {
    screenWidth = Engine::GameEngine::GetInstance().GetScreenWidth();
    screenHeight = Engine::GameEngine::GetInstance().GetScreenHeight();
    mask = nullptr;

    ticks = 0;

    //Pause
    IsPaused = false;
    // Add groups from bottom to top.
    AddNewObject(AnimationGroup = new Group());
    AddNewControlObject(UIGroup = new Group());

    // Start BGM.
    bgmInstance = AudioHelper::PlaySample("play.ogg", true, AudioHelper::BGMVolume);
    backgroundIMG = Engine::Resources::GetInstance().GetBitmap("play/background1.png");
    CreatePauseUI();

    //dialogue=======================================================================
    dialogFont = al_load_font("Resource/fonts/pirulen.ttf", 36, 0);
    if (!dialogFont) {
        throw std::runtime_error("Failed to load font: Resource/fonts/pirulen.ttf");
    }
    std::vector<Dialog> dialogs;
    dialogs.push_back({
        "Hello, adventurer!",
        3.0f,
        "Resource/images/play/arwen.png",
        "Arwen"
    });
    dialogs.push_back({
        "The castle is dangerous!",
        3.0f,
        "Resources/images/play/bryan.png",
        "Bryan"
    });
    StartDialog(dialogs, true);
    //================================================================================
}

void CutScene::Terminate() {
    if (dialogFont) {
        al_destroy_font(dialogFont);
        dialogFont = nullptr;
    }
    AudioHelper::StopSample(bgmInstance);
    IScene::Terminate();
}

void CutScene::Update(float deltaTime) {
    UpdatePauseState();
    if (IsPaused) {
        UIGroup->Update(deltaTime);
        return;
    }
    AnimationGroup->Update(deltaTime);
}
void CutScene::Draw() const {
    if (IsPaused) {
        UIGroup->Draw();
        return;
    }

    IScene::Draw(); // will draw tiles/UI, now offset by camera

    AnimationGroup->Draw();

    //for map debug
    if (currentState == GameState::Dialog) {
        RenderDialog();
    }
}
void CutScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
    if (currentState == GameState::Dialog && dialogSkippable) {
        dialogSkipRequested = true;
    }
}

void CutScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);

    if (IsPaused && keyCode != ALLEGRO_KEY_ESCAPE) return;

    if (keyCode == ALLEGRO_KEY_ESCAPE) {
        IsPaused = !IsPaused;
    }

    if (currentState == GameState::Dialog && dialogSkippable) {
        if (keyCode == ALLEGRO_KEY_SPACE || keyCode == ALLEGRO_KEY_ENTER) {
            dialogSkipRequested = true;
        }
    }
}

//-----------For Pause UI-------------------------

void CutScene::CreatePauseUI() {
    //get width and height
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2, halfH = h / 2;

    //for I forgot
    int h_margin = 150;
    int w_obj = 600, h_obj = 500 + h_margin;

    //button position
    int btnPosX = (w - 400) / 2;
    int btnPosY = (h - h_obj) / 2 + h_margin;

    //label position
    int lblPosX = w/2;
    int lblPosY = (h - h_obj) / 2 + h_margin;


    //outer box bbc and text
    pauseOverlay = new Engine::Image("play/sand.png", (w - w_obj) / 2, (h - h_obj) / 2, w_obj, h_obj);
    pauseOverlay->Visible = false;
    UIGroup->AddNewObject(pauseOverlay);

    pauseText = new Engine::Label("PAUSED", "pirulen.ttf", 48, lblPosX, lblPosY - 100, 0, 0, 100, 255, 0.5, 0.5);
    pauseText->Visible = false;
    UIGroup->AddNewObject(pauseText);

    //to continue perchance
    continueButton = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", btnPosX, btnPosY + 100, 400, 100);
    continueButton->Visible = false;
    continueButton->Enabled = false;
    UIGroup->AddNewControlObject(continueButton);
    continueButton->SetOnClickCallback(std::bind(&CutScene::ContinueOnClick, this, 1));

    //exit to stage scene
    exitButton = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", btnPosX, btnPosY + 360, 400, 100);
    exitButton->Visible = false;
    exitButton->Enabled = false;
    UIGroup->AddNewControlObject(exitButton);
    exitButton->SetOnClickCallback(std::bind(&CutScene::BackOnClick, this, 1));

    //the label
    continueLabel = new Engine::Label("Continue", "pirulen.ttf", 48, lblPosX, lblPosY + 150, 0, 0, 0, 255, 0.5, 0.5);
    continueLabel->Visible = false;
    UIGroup->AddNewObject(continueLabel);

    exitLabel = new Engine::Label("Exit", "pirulen.ttf", 48, lblPosX, lblPosY + 410, 0, 0, 0, 255, 0.5, 0.5);
    exitLabel->Visible = false;
    UIGroup->AddNewObject(exitLabel);

    //slider
    sliderBGM = new Slider(40 + halfW - 95, halfH - 200 - 2, 190, 4);
    sliderBGM->Visible = false;
    sliderBGM->Enabled = false;
    sliderBGM->SetValue(AudioHelper::BGMVolume);
    sliderBGM->SetOnValueChangedCallback(std::bind(&CutScene::BGMSlideOnValueChanged, this, std::placeholders::_1));
    UIGroup->AddNewControlObject(sliderBGM);

    BGMSlider = new Engine::Label("BGM: ", "pirulen.ttf", 28, 40 + halfW - 60 - 95, halfH - 200, 0, 0, 0, 255, 0.5, 0.5);
    BGMSlider->Visible = false;
    UIGroup->AddNewObject(BGMSlider);

    //slider
    sliderSFX = new Slider(40 + halfW - 95, halfH - 150 + 2, 190, 4);
    sliderSFX->Visible = false;
    sliderSFX->Enabled = false;
    sliderSFX->SetValue(AudioHelper::SFXVolume);
    sliderSFX->SetOnValueChangedCallback(std::bind(&CutScene::SFXSlideOnValueChanged, this, std::placeholders::_1));
    UIGroup->AddNewControlObject(sliderSFX);

    SFXSlider = new Engine::Label("SFX: ", "pirulen.ttf", 28, 40 + halfW - 60 - 95, halfH - 150, 0, 0, 0, 255, 0.5, 0.5);
    SFXSlider->Visible = false;
    UIGroup->AddNewObject(SFXSlider);

}

//-------For Exit, restart, and Continue Button------------------
void CutScene::BackOnClick(int state) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}
void CutScene::ContinueOnClick(int state) {
    IsPaused = false;
}
//volume
void CutScene::BGMSlideOnValueChanged(float value) {
    AudioHelper::BGMVolume = value;
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
}
void CutScene::SFXSlideOnValueChanged(float value) {
    AudioHelper::SFXVolume = value;
}

//---For Pause---------------->>>>>>>>>>>>>>>>>>
void CutScene::UpdatePauseState() {
    bool show = IsPaused;
    if (pauseOverlay) {
        pauseOverlay->Visible = show;
        pauseText->Visible = show;
    }
    if (continueButton) {
        continueButton->Visible = show;
        continueButton->Enabled = show;
        continueLabel->Visible = show;
    }
    if (exitButton) {
        exitButton->Visible = show;
        exitButton->Enabled = show;
        exitLabel->Visible = show;
    }
    if (sliderBGM) {
        sliderBGM->Visible = show;
        sliderBGM->Enabled = show;
        BGMSlider->Visible = show;
    }
    if (sliderSFX) {
        sliderSFX->Visible = show;
        sliderSFX->Enabled = show;
        SFXSlider->Visible = show;
    }
}

void CutScene::RenderDialog() const {
    if (currentDialogText.empty()) return;

    const int screenW = Engine::GameEngine::GetInstance().GetScreenWidth();
    const int screenH = Engine::GameEngine::GetInstance().GetScreenHeight();

    // Draw a semi-transparent box
    al_draw_filled_rectangle(screenW * 0.1, screenH * 0.8, screenW * 0.9, screenH * 0.95, al_map_rgba(0, 0, 0, 200));
    al_draw_rectangle(screenW * 0.1, screenH * 0.8, screenW * 0.9, screenH * 0.95, al_map_rgb(255, 255, 255), 2);

    // Draw speaker image (left side of dialog box)
    if (!currentSpeakerImagePath.empty()) {
        ALLEGRO_BITMAP* speakerImg = al_load_bitmap(currentSpeakerImagePath.c_str());
        if (speakerImg) {
            // Calculate position and size (adjust these values as needed)
            const float imgWidth = PlayScene::BlockSize;
            const float imgHeight = PlayScene::BlockSize;
            const float imgX = screenW * 0.1f - imgWidth - 10; // Left of dialog box
            const float imgY = screenH * 0.8f;

            al_draw_scaled_bitmap(speakerImg,
                                0, 0, al_get_bitmap_width(speakerImg), al_get_bitmap_height(speakerImg),
                                imgX, imgY, imgWidth, imgHeight, 0);
            al_destroy_bitmap(speakerImg);
        }
    }

    // Draw speaker name (optional)
    if (!currentSpeakerName.empty() && dialogFont) {
        al_draw_text(dialogFont, al_map_rgb(255, 255, 255),
                    screenW * 0.15, screenH * 0.81,
                    ALLEGRO_ALIGN_LEFT, currentSpeakerName.c_str());
    }

    // Draw dialog text
    if (dialogFont) {
        al_draw_text(dialogFont, al_map_rgb(255, 255, 255),
                    screenW * 0.5, screenH * 0.825,
                    ALLEGRO_ALIGN_CENTER, currentDialogText.c_str());
    }
    if (dialogSkippable && dialogFont) {
        std::string hint = dialogFastForward ?
            "Press to skip >>" : "Press to continue...";

        al_draw_text(dialogFont, al_map_rgba(200, 200, 200, 200),
                    screenW * 0.88, screenH * 0.92,
                    ALLEGRO_ALIGN_RIGHT, hint.c_str());
    }
}

void CutScene::StartDialog(const std::vector<Dialog>& dialogs, bool skippable = true) {
    dialogQueue = std::queue<Dialog>(); // Clear existing
    for (const auto& d : dialogs) dialogQueue.push(d);

    currentState = GameState::Dialog;
    dialogSkippable = skippable;
    dialogSkipRequested = false;
    dialogFastForward = false;

    if (!dialogQueue.empty()) {
        const Dialog& first = dialogQueue.front();
        currentDialogText = first.text;
        currentSpeakerImagePath = first.speakerImagePath;
        currentSpeakerName = first.speakerName;
        dialogTimer = first.duration;
        dialogQueue.pop();
    }
}

void CutScene::UpdateDialog(float deltaTime) {
    if (currentState != GameState::Dialog) return;

    // Handle skip input
    if (dialogSkipRequested) {
        if (dialogFastForward) {
            // Second press - skip entire dialog
            dialogQueue = std::queue<Dialog>(); // Clear remaining dialogs
            dialogTimer = 0;
        } else {
            // First press - show full text immediately
            dialogFastForward = true;
            dialogTimer = 0;
        }
        dialogSkipRequested = false;
    }

    // Update normally or fast-forward
    if (!dialogFastForward) {
        dialogTimer -= deltaTime;
    } else {
        dialogTimer = 0;
    }

    // Advance dialog
    if (dialogTimer <= 0) {
        dialogFastForward = false; // Reset fast-forward for next dialog

        if (!dialogQueue.empty()) {
            const Dialog& next = dialogQueue.front();
            currentDialogText = next.text;
            currentSpeakerImagePath = next.speakerImagePath;
            currentSpeakerName = next.speakerName;
            dialogTimer = next.duration;
            dialogQueue.pop();
        } else {
            // Dialog ended
            currentState = GameState::Normal;
            currentDialogText.clear();
            currentSpeakerImagePath.clear();
            currentSpeakerName.clear();
        }
    }
}

