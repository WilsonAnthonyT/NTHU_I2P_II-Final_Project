#ifndef CUTSCENE_H
#define CUTSCENE_H

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <functional>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <queue>
#include <map>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Player/Player.h"
#include "UI/Component/Slider.hpp"
#include "InteractiveBlock/Door.h"
#include "InteractiveBlock/Sensor.h"

namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}

class CutScene : public Engine::IScene {
public:
    float Blocksize;
    // Dialog structure
    struct Dialog {
        std::string text;
        float duration;
        std::string speakerImagePath;
        std::string speakerName;
    };

    // Animation types
    enum class AnimationType {
        FADE_IN,
        FADE_OUT,
        SLIDE_IN,
        SLIDE_OUT,
        CUSTOM
    };
    struct SceneTransition {
        float delay = 0.0f;
        std::string targetScene = "";
        AnimationType transitionType = AnimationType::FADE_OUT;
        float duration = 1.0f;
        float DialogDuration = 0.0f;
        float AnimationDuration = 0.0f;
    };


    // Enhanced AnimatedSprite class
    class AnimatedSprite {
    private:

        float frameDuration;
        float currentTime = 0;

        bool looping = true;
        bool flipHorizontal = false;
        float scaleX = 1.0f;
        float scaleY = 1.0f;
        ALLEGRO_COLOR tint = al_map_rgba_f(1, 1, 1, 1);

    public:
        int currentFrame = 0;
        std::vector<std::shared_ptr<ALLEGRO_BITMAP>> frames;
        AnimatedSprite(const std::vector<std::string>& framePaths, float fps, bool loop = true);
        void Update(float deltaTime);
        void Draw(float x, float y) const;
        void Reset();
        bool IsComplete() const;
        void SetFlip(bool flip) { flipHorizontal = flip; }
        void SetScale(float x, float y) { scaleX = x; scaleY = y; }
        void SetScale(float uniformScale) { scaleX = scaleY = uniformScale; }
        void SetTint(ALLEGRO_COLOR color) { tint = color; }
    };

    // Tween class for smooth animations
    class Tween {
    private:
        float startValue;
        float endValue;
        float duration;
        float elapsed = 0;
        std::function<float(float)> easing;
        bool completed = false;
        
    public:
        Tween(float start, float end, float dur, std::function<float(float)> ease);
        float Update(float deltaTime);
        bool IsComplete() const;
        void Reset();
    };

    struct Character {
        std::unique_ptr<AnimatedSprite> animation;
        Engine::Point position;
        Engine::Point TargetPosition;
        Engine::Point TargetScale;
        std::unique_ptr<Tween> tweenX;
        std::unique_ptr<Tween> tweenY;
        float scaleX = 1.0f;
        float scaleY = 1.0f;
        bool flipHorizontal = false;
        ALLEGRO_COLOR tint = al_map_rgba_f(1, 1, 1, 1);
    };


    // Public members
    ALLEGRO_FONT* dialogFont = nullptr;
    bool dialogSkipRequested = false;
    bool dialogSkippable = true;
    bool dialogFastForward = false;
    std::string currentSpeakerImagePath;
    std::string currentSpeakerName;
    std::queue<Dialog> dialogQueue;
    std::string currentDialogText;
    float dialogTimer = 0.0f;
    ALLEGRO_BITMAP* mask;
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    
    explicit CutScene();
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;

    //Animation control2
    void AddCharacterAnimation(const std::string& characterId,
                            const std::vector<std::string>& framePaths,
                            float fps, bool loop = true);
    void PlayCharacterAnimation(const std::string& characterId, const std::string& animName);
    void SetCharacterPosition(const std::string& characterId, float x, float y);
    void MoveCharacterTo(const std::string& characterId, float targetX, float targetY, float duration);



    // Scene transition
    void SetSceneTransition(const SceneTransition& transition);
    void StartFadeOut();

    //Scaling
    void SetCharacterScale(const std::string& characterId, float scaleX, float scaleY);
    void SetCharacterUniformScale(const std::string& characterId, float scale);
    void ScaleCharacterTo(const std::string& charId, float targetScaleX, float targetScaleY, float duration);

    void SetTint(const std::string& charId, int r, int g, int b);

private:
    PlayScene *scene;
    // Pause menu
    bool IsPaused = false;
    void UpdatePauseState();
    void CreatePauseUI();
    
    // Dialog system
    std::vector<Dialog> dialogs;
    bool isDialogStarted = false;
    bool isDialogFinished = false;
    void StartDialog(const std::vector<Dialog>& dialogs, bool skippable);
    void UpdateDialog(float deltaTime);
    void RenderDialog() const;

    //Animation2
    std::unordered_map<std::string, Character> characterAnimations;
    std::unordered_map<std::string, Engine::Point> characterPositions;
    std::unordered_map<std::string, std::unique_ptr<Tween>> characterTweensX;
    std::unordered_map<std::string, std::unique_ptr<Tween>> characterTweensY;
    std::unordered_map<std::string, std::unique_ptr<Tween>> characterScaleTweensX;
    std::unordered_map<std::string, std::unique_ptr<Tween>> characterScaleTweensY;

    // Animation system
    //std::map<std::string, std::vector<std::string>> animations;

    // Scene transition
    SceneTransition sceneTransition;
    float transitionTimer = 0.0f;
    bool transitionStarted = false;
    float fadeAlpha = 0.0f;
    std::unique_ptr<Tween> fadeTween;

    // Game state
    enum class GameState {
        Normal,
        Dialog,
        Transitioning
    };
    GameState currentState = GameState::Normal;

    // UI Elements
    int screenWidth;
    int screenHeight;
    int StageId;
    float ticks;
    Group *AnimationGroup;
    Group *UIGroup;
    
    Engine::Image *pauseOverlay;
    Engine::Label *pauseText;
    Engine::ImageButton *continueButton;
    Engine::ImageButton *exitButton;
    Engine::Label *continueLabel;
    Engine::Label *exitLabel;
    Engine::Label *BGMSlider;
    Engine::Label *SFXSlider;
    Slider *sliderBGM, *sliderSFX;

    // Callbacks
    void BackOnClick(int state);
    void ContinueOnClick(int state);
    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);

    // Helper functions
    void UpdateAnimations(float deltaTime);
    void UpdateTransitions(float deltaTime);
    void DrawTransitionEffect() const;

    bool startAnimation;
    bool startDialog;
    bool startTransition;

    bool isAnimationPhaseDone = false;
};

#endif // CUTSCENE_H