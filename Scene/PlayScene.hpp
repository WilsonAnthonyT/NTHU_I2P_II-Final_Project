#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "UI/Component/Slider.hpp"

class Turret;
namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}   // namespace Engine

class PlayScene final : public Engine::IScene {
private:
    Engine::Point Camera;
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    int screenWidth;
    int screenHeight;

protected:
    int lives;
    int money;
    int SpeedMult;

public:
    enum TileType {
        TILE_AIR,
        TILE_DIRT,
        TILE_WPLATFORM,
    };
    static const float Gravity;
    static bool DebugMode;
    static bool Pause;
    static const std::vector<Engine::Point> directions;
    static int MapWidth, MapHeight;
    static const int BlockSize;
    static const float DangerTime;
    static Engine::Point SpawnGridPoint;
    static const std::vector<int> code;
    int MapId;
    float ticks;
    float deathCountDown;
    // Map tiles.
    Group *TileMapGroup;
    Group *GroundEffectGroup;
    Group *DebugIndicatorGroup;
    Group *BulletGroup;
    Group *PlayerGroup;
    Group *EnemyGroup;
    Group *EffectGroup;
    Group *UIGroup;
    Engine::Label *UIMoney;
    Engine::Label *UILives;
    Engine::Image *imgTarget;
    Engine::Sprite *dangerIndicator;
    Turret *preview;
    std::vector<std::vector<TileType>> mapState;
    std::vector<std::vector<int>> mapDistance;
    std::list<std::pair<int, float>> enemyWaveData;
    std::list<int> keyStrokes;
    static Engine::Point GetClientSize();
    explicit PlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void OnKeyDown(int keyCode) override;
    int GetMoney() const;
    void EarnMoney(int money);
    void ReadMap();
    void ReadEnemyWave();
    void ConstructUI();
    void UIBtnClicked(int id);
    void HomeOnClick(int id);
    // void ModifyReadMapTiles();

    //----Pause-----
    bool IsPaused = false;
    void UpdatePauseState();
    void CreatePauseUI();
    //----
    Engine::Image *pauseOverlay;
    Engine::Label *pauseText;

    Engine::ImageButton *continueButton;
    Engine::ImageButton *restartButton;
    Engine::ImageButton *exitButton;

    Engine::Label *continueLabel;
    Engine::Label *exitLabel;
    Engine::Label *restartLabel;
    Engine::Label *BGMSlider;
    Engine::Label *SFXSlider;

    Slider *sliderBGM, *sliderSFX;
    //----
    void BackOnClick(int state);
    void ContinueOnClick(int state);
    void RestartOnClick(int state);
    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);
};
#endif   // PLAYSCENE_HPP
