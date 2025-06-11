//
// Created by MiHu on 6/11/2025.
//

#include "CutScene.h"

#include <allegro5/allegro_primitives.h>

#include "StageSelectScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Label.hpp"

#include "CutScene.h"

#include <iostream>
#include <ostream>
#include <allegro5/allegro_primitives.h>
#include "StageSelectScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Label.hpp"

// AnimatedSprite implementation==============================================================
CutScene::AnimatedSprite::AnimatedSprite(const std::vector<std::string>& framePaths, float fps, bool loop)
    : frameDuration(1.0f/fps), looping(loop) {
    for (const auto& path : framePaths) {
        frames.push_back(Engine::Resources::GetInstance().GetBitmap(path));
    }
}

void CutScene::AnimatedSprite::Update(float deltaTime) {
    currentTime += deltaTime;
    if (currentTime >= frameDuration) {
        currentTime = 0;
        currentFrame++;
        if (currentFrame >= frames.size()) {
            if (looping) currentFrame = 0;
            else currentFrame = frames.size() - 1;
        }
    }
}

void CutScene::AnimatedSprite::Draw(float x, float y) const {
    if (!frames.empty() && currentFrame < frames.size()) {
        int flags = flipHorizontal ? ALLEGRO_FLIP_HORIZONTAL : 0;
        al_draw_tinted_scaled_bitmap(
            frames[currentFrame].get(),
            tint,
            0, 0,
            al_get_bitmap_width(frames[currentFrame].get()),
            al_get_bitmap_height(frames[currentFrame].get()),
            x, y,
            al_get_bitmap_width(frames[currentFrame].get()) * scaleX,
            al_get_bitmap_height(frames[currentFrame].get()) * scaleY,
            flags
        );
    }
}

void CutScene::AnimatedSprite::Reset() {
    currentFrame = 0;
    currentTime = 0;
}

bool CutScene::AnimatedSprite::IsComplete() const {
    return !looping && currentFrame == frames.size() - 1;
}

// Tween implementation===============================================================================================
CutScene::Tween::Tween(float start, float end, float dur, std::function<float(float)> ease)
    : startValue(start), endValue(end), duration(dur), easing(ease) {}

float CutScene::Tween::Update(float deltaTime) {
    if (completed) return endValue;

    elapsed += deltaTime;
    if (elapsed >= duration) {
        completed = true;
        return endValue;
    }

    float t = easing(elapsed / duration);
    return startValue + (endValue - startValue) * t;
}

bool CutScene::Tween::IsComplete() const {
    return completed;
}

void CutScene::Tween::Reset() {
    elapsed = 0;
    completed = false;
}

// CutScene implementation
CutScene::CutScene() : sceneTransition{0.0f, "play", AnimationType::FADE_OUT, 1.0f} {}

void CutScene::Initialize() {
    transitionStarted = false;
    transitionTimer = 0.0f;
    fadeAlpha = 0.0f;
    currentState = GameState::Normal;
    isDialogStarted = false;
    isDialogFinished = false;
    isAnimationPhaseDone = false;

    characterTweensX.clear();
    characterTweensY.clear();
    characterScaleTweensX.clear();
    characterScaleTweensY.clear();
    dialogs.clear();

    scene = dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"));
    screenWidth = Engine::GameEngine::GetInstance().GetScreenWidth();
    screenHeight = Engine::GameEngine::GetInstance().GetScreenHeight();
    Blocksize = screenWidth / 16;
    mask = nullptr;
    ticks = 0;
    IsPaused = false;

    // Add groups from bottom to top
    AddNewObject(AnimationGroup = new Group());
    AddNewControlObject(UIGroup = new Group());

    // Start BGM
    bgmInstance = AudioHelper::PlaySample("play.ogg", true, AudioHelper::BGMVolume);
    CreatePauseUI();

    // Initialize dialog system
    dialogFont = al_load_font("Resource/fonts/pirulen.ttf",  Blocksize * 0.2666666666667f, 0);
    if (!dialogFont) {
        throw std::runtime_error("Failed to load font: Resource/fonts/pirulen.ttf");
    }

    // Example dialog
    if (scene->MapId == 1) {
        backgroundIMG = Engine::Resources::GetInstance().GetBitmap("cut-scene/earth.png");
        PlayScene::Camera.x = 0;
        PlayScene::Camera.y = 0;
        PlayScene::MapWidth = Engine::GameEngine::GetInstance().GetScreenWidth();
        PlayScene::MapHeight = Engine::GameEngine::GetInstance().GetScreenHeight();
        // Example animation
        dialogs.push_back({
           "Warning unknown ships has entered our area.",
           3.5f,
           "play/bryan.png",
           "Bryan"
       });
        dialogs.push_back({
            "These ships descended without a warning.",
            4.0f,
            "play/tool-base.png",
            " "
        });
        dialogs.push_back({
            "We couldn’t stop them.",
            4.0f,
            "play/tool-base.png",
            " "
        });
        //tumbal ga bakal kepake
        dialogs.push_back({
            "Feat. Bryan and Arwen",
            4.0f,
            "play/tool-base.png",
            " "
        });
        //Char 1 ship
        std::vector<std::string> shipFrames = {
            "cut-scene/ship1.png"
        };
        AddCharacterAnimation("ship1", shipFrames, 10.0f, true);
        SetCharacterScale("ship1", 2.0f, 2.0f);
        SetCharacterPosition("ship1", screenWidth, PlayScene::BlockSize * 3);
        MoveCharacterTo("ship1", screenWidth/2, PlayScene::BlockSize * 3, 6.0f);
        ScaleCharacterTo("ship1", 0.8f, 0.8f, 6.0f);

        sceneTransition.delay = 23.0f;
        sceneTransition.targetScene = "play";
        sceneTransition.transitionType = AnimationType::FADE_OUT;
        sceneTransition.duration = 1.0f;

    }
    else if (scene->MapId == 2) {
        backgroundIMG = Engine::Resources::GetInstance().GetBitmap("cut-scene/cutscenebg1.png");
        PlayScene::Camera.x = 0;
        PlayScene::Camera.y = 0;
        PlayScene::MapWidth = Engine::GameEngine::GetInstance().GetScreenWidth();
        PlayScene::MapHeight = Engine::GameEngine::GetInstance().GetScreenHeight();
        // Example animation
        dialogs.push_back({
           "The sky... it's burning. They’ve arrived.",
           3.5f,
           "play/arwen.png",
           "Arwen"
       });
        dialogs.push_back({
            "Unknown ships descended without a warning. We couldn’t stop them.",
            4.0f,
            "play/bryan.png",
            "Bryan"
        });
        dialogs.push_back({
            "We’ve lost contact with half the world. They're not here to talk.",
            3.5f,
            "play/arwen.png",
            "Arwen"
        });
        dialogs.push_back({
            "HQ’s gone silent... We're on our own now.",
            3.0f,
            "play/bryan.png",
            "Bryan"
        });
        dialogs.push_back({
            "This is no longer a war... It’s survival.",
            3.0f,
            "play/arwen.png",
            "Arwen"
        });
        dialogs.push_back({
            "Then we fight. For Earth, for everyone who still breathes.",
            3.0f,
            "play/bryan.png",
            "Bryan"
        });
        //Char 1 ship
        std::vector<std::string> shipFrames = {
            "cut-scene/ship1.png"
        };
        AddCharacterAnimation("ship1", shipFrames, 10.0f, true);
        SetCharacterScale("ship1", 2.0f, 2.0f);
        SetCharacterPosition("ship1", PlayScene::BlockSize * 15 , PlayScene::BlockSize * 3);
        MoveCharacterTo("ship1", PlayScene::BlockSize * 5, PlayScene::BlockSize * 3, 6.0f);
        ScaleCharacterTo("ship1", 0.8f, 0.8f, 6.0f);
        StartDialog(dialogs, true);

        sceneTransition.delay = 23.0f;
        sceneTransition.targetScene = "play";
        sceneTransition.transitionType = AnimationType::FADE_OUT;
        sceneTransition.duration = 1.0f;
    }
    else if (scene->MapId == 3) {
        sceneTransition.delay = 0.001f;
        sceneTransition.targetScene = "play";
        sceneTransition.transitionType = AnimationType::FADE_OUT;
        sceneTransition.duration = 0.001f;
    }
    else if (scene->MapId == 4) {
        sceneTransition.delay = 0.001f;
        sceneTransition.targetScene = "play";
        sceneTransition.transitionType = AnimationType::FADE_OUT;
        sceneTransition.duration = 0.001f;
    }
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

    // Update game state
    switch (currentState) {
        case GameState::Dialog:
            UpdateDialog(deltaTime);
            if (dialogQueue.empty() && !isDialogFinished) {
                isDialogFinished = true;
                currentState = GameState::Transitioning;
                std::cout << "Dialog finished naturally" << std::endl;
            }
            break;
        case GameState::Transitioning:
            UpdateTransitions(deltaTime);
            break;
        case GameState::Normal:
            if (!isAnimationPhaseDone) {
                for (auto& [id, pos] : characterAnimations) {
                    if (characterTweensX.count(id) && !characterTweensX[id]->IsComplete()) {
                        pos.position.x = characterTweensX[id]->Update(deltaTime);
                    }
                    if (characterTweensY.count(id) && !characterTweensY[id]->IsComplete()) {
                        pos.position.y = characterTweensY[id]->Update(deltaTime);
                    }
                }

                for (auto& [id, character] : characterAnimations) {
                    character.animation->Update(deltaTime);
                    if (characterScaleTweensX.find(id) != characterScaleTweensX.end() &&
                        !characterScaleTweensX[id]->IsComplete()) {
                        character.scaleX = characterScaleTweensX[id]->Update(deltaTime);
                        }
                    if (characterScaleTweensY.find(id) != characterScaleTweensY.end() &&
                        !characterScaleTweensY[id]->IsComplete()) {
                        character.scaleY = characterScaleTweensY[id]->Update(deltaTime);
                        }
                }
                bool allAnimationsDone = true;
                for (const auto& [id, character] : characterAnimations) {
                    if (!character.animation->IsComplete()) {
                        allAnimationsDone = false;
                        break;
                    }
                }
                if (allAnimationsDone) {
                    isAnimationPhaseDone = true;
                    if (scene->MapId == 1) {
                        StartDialog(dialogs, true);
                        isDialogStarted = true;
                    }
                }
            }
            if (isAnimationPhaseDone) {
                if (scene->MapId == 1) {
                    if (!isDialogStarted) {
                        currentState = GameState::Dialog;
                        isDialogStarted = true;
                    }
                }
                else {
                    sceneTransition.delay = 0.01f;
                }
            }
            break;
    }


    // Check for scene transition
    if (!transitionStarted && sceneTransition.delay > 0.0f) {
        transitionTimer += deltaTime;

        //only for lv 1=================================
        if (scene->MapId == 1) {
            if (transitionTimer >= 6.0f && !isDialogStarted) {
                StartDialog(dialogs, true);
                isDialogStarted = true;
            }
            if (isDialogFinished) {
                sceneTransition.delay = 0.1f;
            }
        }
        else {
            if (isAnimationPhaseDone) {
                sceneTransition.delay = 0.01f;
            }
        }
        //============================================
        if (transitionTimer >= sceneTransition.delay) {
            StartFadeOut();
        }
    }

    AnimationGroup->Update(deltaTime);
}

void CutScene::Draw() const {
    if (IsPaused) {
        UIGroup->Draw();
        return;
    }

    // ALLEGRO_TRANSFORM trans;
    // al_identity_transform(&trans);
    // al_translate_transform(&trans, -PlayScene::Camera.x, -PlayScene::Camera.y);  // apply camera offset
    // al_use_transform(&trans);  // set transform for all following drawing

    IScene::Draw();

    switch (currentState) {
        case GameState::Dialog:
            if (scene->MapId == 1) {
                for (const auto& [id, character] : characterAnimations) {
                    if (character.animation) {
                        character.animation->SetFlip(character.flipHorizontal);
                        character.animation->SetScale(character.TargetScale.x, character.TargetScale.y);
                        character.animation->SetTint(character.tint);
                        character.animation->Draw(character.TargetPosition.x, character.TargetPosition.y);
                    }
                }
            }
            else {
                for (const auto& [id, character] : characterAnimations) {
                    if (character.animation) {
                        character.animation->SetFlip(character.flipHorizontal);
                        character.animation->SetScale(character.scaleX, character.scaleY);
                        character.animation->SetTint(character.tint);
                        character.animation->Draw(character.position.x, character.position.y);
                    }
                }
            }
            RenderDialog();
            break;
        case GameState::Transitioning:
            if (scene->MapId == 1) {
                for (const auto& [id, character] : characterAnimations) {
                    if (character.animation) {
                        character.animation->SetFlip(character.flipHorizontal);
                        character.animation->SetScale(character.TargetScale.x, character.TargetScale.y);
                        character.animation->SetTint(character.tint);
                        character.animation->Draw(character.TargetPosition.x, character.TargetPosition.y);
                    }
                }
            }
            else {
                for (const auto& [id, character] : characterAnimations) {
                    if (character.animation) {
                        character.animation->SetFlip(character.flipHorizontal);
                        character.animation->SetScale(character.TargetScale.x, character.TargetScale.y);
                        character.animation->SetTint(character.tint);
                        character.animation->Draw(character.TargetPosition.x, character.TargetPosition.y);
                    }
                }
            }
            DrawTransitionEffect();
            break;
        case GameState::Normal:
            for (const auto& [id, character] : characterAnimations) {
                if (character.animation) {
                    character.animation->SetFlip(character.flipHorizontal);
                    character.animation->SetScale(character.scaleX, character.scaleY);
                    character.animation->SetTint(character.tint);
                    character.animation->Draw(character.position.x, character.position.y);
                }
            }
            if (!isAnimationPhaseDone) {
                al_draw_text(
                    dialogFont,
                    al_map_rgba(255, 255, 255, 180),
                    screenWidth - 20,
                    20,
                    ALLEGRO_ALIGN_RIGHT,
                    "Press [SPACE] to Skip"
                );
            }
            break;
    }
}

void CutScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
    if (currentState == GameState::Dialog && dialogSkippable) {
        dialogSkipRequested = true;
    }
    if (currentState == GameState::Normal && !isAnimationPhaseDone) {
        isAnimationPhaseDone = true; // Skip animations
        if (scene->MapId == 1) StartDialog(dialogs, true); // Start dialog
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
    if (currentState == GameState::Normal && !isAnimationPhaseDone) {
        if (keyCode == ALLEGRO_KEY_SPACE) isAnimationPhaseDone = true; // Skip animations
        if (scene->MapId == 1) {
            StartDialog(dialogs, true);
            isDialogStarted = true;
        }
    }
}

// Animation control

// Scene transition
void CutScene::SetSceneTransition(const SceneTransition& transition) {
    sceneTransition = transition;
}

void CutScene::StartFadeOut() {
    if (transitionStarted) return;

    transitionStarted = true;
    currentState = GameState::Transitioning;
    fadeAlpha = 0.0f;

    fadeTween = std::make_unique<Tween>(0.0f, 1.0f, sceneTransition.duration,
        [](float t) { return t * t; }); // Ease in
}

void CutScene::UpdateTransitions(float deltaTime) {
    if (fadeTween && !fadeTween->IsComplete()) {
        fadeAlpha = fadeTween->Update(deltaTime);

        if (fadeTween->IsComplete()) {
            Engine::GameEngine::GetInstance().ChangeScene(sceneTransition.targetScene);
        }
    }
}

void CutScene::DrawTransitionEffect() const {
    if (sceneTransition.transitionType == AnimationType::FADE_OUT) {
        al_draw_filled_rectangle(0, 0, screenWidth, screenHeight,
            al_map_rgba_f(0, 0, 0, fadeAlpha));
    }
}

// Animation control for multiple characters============================
void CutScene::AddCharacterAnimation(const std::string& characterId,
                                   const std::vector<std::string>& framePaths,
                                   float fps, bool loop) {
    animations[characterId] = framePaths;
    characterAnimations[characterId].animation = std::make_unique<AnimatedSprite>(framePaths, fps, loop);
}

void CutScene::PlayCharacterAnimation(const std::string& characterId, const std::string& animName) {
    if (animations.find(animName) != animations.end()) {
        characterAnimations[characterId].animation = std::make_unique<AnimatedSprite>(animations[animName], 10.0f, true);
    }
}

void CutScene::SetCharacterPosition(const std::string& characterId, float x, float y) {
    characterAnimations[characterId].position = Engine::Point(x, y);
}

void CutScene::MoveCharacterTo(const std::string& characterId, float targetX, float targetY, float duration) {
    characterAnimations[characterId].TargetPosition.x = targetX;
    characterAnimations[characterId].TargetPosition.y = targetY;

    Engine::Point currentPos = characterAnimations[characterId].position;
    characterTweensX[characterId] = std::make_unique<Tween>(currentPos.x, targetX, duration,
        [](float t) { return t * t * (3 - 2 * t); });
    characterTweensY[characterId] = std::make_unique<Tween>(currentPos.y, targetY, duration,
        [](float t) { return t * t * (3 - 2 * t); });
}

void CutScene::SetCharacterScale(const std::string& charId, float scaleX, float scaleY) {
    if (characterAnimations.find(charId) != characterAnimations.end()) {
        characterAnimations[charId].scaleX = scaleX;
        characterAnimations[charId].scaleY = scaleY;
    }
}

void CutScene::SetCharacterUniformScale(const std::string& charId, float scale) {
    if (characterAnimations.find(charId) != characterAnimations.end()) {
        characterAnimations[charId].scaleX = scale;
        characterAnimations[charId].scaleY = scale;
    }
}

void CutScene::ScaleCharacterTo(const std::string& charId, float targetScaleX, float targetScaleY, float duration) {
    if (characterAnimations.find(charId) == characterAnimations.end()) return;

    characterAnimations[charId].TargetScale.x = targetScaleX;
    characterAnimations[charId].TargetScale.y = targetScaleY;

    auto& character = characterAnimations[charId];
    characterScaleTweensX[charId] = std::make_unique<Tween>(
        character.scaleX, targetScaleX, duration,
        [](float t) { return t * t * (3 - 2 * t); }); // Smoothstep
    characterScaleTweensY[charId] = std::make_unique<Tween>(
        character.scaleY, targetScaleY, duration,
        [](float t) { return t * t * (3 - 2 * t); });
}

void CutScene::SkipAnimationToEnd() {
    for (auto& [id, character] : characterAnimations) {
        if (characterAnimations.count(id)) {
            // Teleport position
            character.position.x = characterAnimations[id].TargetPosition.x;
            character.position.y = characterAnimations[id].TargetPosition.y;

            // Teleport scale
            character.scaleX = characterAnimations[id].TargetScale.x;
            character.scaleY = characterAnimations[id].TargetScale.y;

        }

        // Force the animation to its last frame
        if (character.animation && !character.animation->IsComplete()) {
            character.animation->currentFrame = character.animation->frames.size() - 1;
        }
    }

    isAnimationPhaseDone = true; // Mark animations as complete
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
        auto speakerImg = Engine::Resources::GetInstance().GetBitmap(currentSpeakerImagePath).get();
        if (speakerImg) {
            // Calculate position and size (adjust these values as needed)
            const float imgWidth = PlayScene::BlockSize;
            const float imgHeight = PlayScene::BlockSize;
            const float imgX = screenW * 0.1f - imgWidth - 10; // Left of dialog box
            const float imgY = screenH * 0.8f;
            al_draw_scaled_bitmap(speakerImg,
                                0, 0, al_get_bitmap_width(speakerImg), al_get_bitmap_height(speakerImg),
                                imgX, imgY, imgWidth, imgHeight, 0);
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
                    screenW * 0.5, screenH * 0.85,
                    ALLEGRO_ALIGN_CENTER, currentDialogText.c_str());
    }
    if (dialogSkippable && dialogFont) {
        std::string hint = dialogFastForward ?
            "Press to skip >>" : "Press to continue...";

        al_draw_text(dialogFont, al_map_rgba(200, 200, 200, 200),
                    screenW * 0.88, screenH * 0.91,
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



