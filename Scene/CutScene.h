//
// Created by MiHu on 6/11/2025.
//

#ifndef CUTSCENE_H
#define CUTSCENE_H

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <queue>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Player/Player.h"
#include "UI/Component/Slider.hpp"

#include "InteractiveBlock/Door.h"
#include "InteractiveBlock/Sensor.h"

namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}   // namespace Engine


class CutScene : public Engine::IScene{
public:
    ALLEGRO_FONT* dialogFont = nullptr;
    struct Dialog {
        std::string text;
        float duration;
        std::string speakerImagePath;  // Path to the speaker's image
        std::string speakerName;       // Optional: speaker's name
    };
    bool dialogSkipRequested = false;
    bool dialogSkippable = true;
    bool dialogFastForward = false;
    std::string currentSpeakerImagePath;
    std::string currentSpeakerName;
    std::queue<Dialog> dialogQueue;
    std::string currentDialogText;
    float dialogTimer = 0.0f;

    //====================================
    ALLEGRO_BITMAP* mask;
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    int screenWidth;
    int screenHeight;
    int StageId;
    float ticks;
    Group *AnimationGroup;
    Group *UIGroup;
    explicit CutScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;

    void OnKeyDown(int keyCode) override;

    //----Pause-----
    bool IsPaused = false;
    void UpdatePauseState();
    void CreatePauseUI();
    //----
    Engine::Image *pauseOverlay;
    Engine::Label *pauseText;

    Engine::ImageButton *continueButton;
    Engine::ImageButton *exitButton;

    Engine::Label *continueLabel;
    Engine::Label *exitLabel;
    Engine::Label *BGMSlider;
    Engine::Label *SFXSlider;

    Slider *sliderBGM, *sliderSFX;
    //----
    void BackOnClick(int state);
    void ContinueOnClick(int state);
    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);
    void StartDialog(const std::vector<Dialog>& dialogs, bool skippable);
    void UpdateDialog(float deltaTime);
    void RenderDialog() const;
    enum class GameState {
        Normal,
        Dialog
    };
    GameState currentState = GameState::Normal;
};



#endif //CUTSCENE_H
