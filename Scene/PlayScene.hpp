#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <list>
#include <memory>
#include <utility>
#include <vector>
#include <queue>
#include <sstream>

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


class Sensor;
class PlayScene final : public Engine::IScene {
private:
    struct Wave {
        //spawn point y, type, count, cooldown, left/right (1/0)
        float position_y;
        float type;
        float count;
        float cooldown;
        float direction;
    };
    std::vector<Wave> enemyWave;
    float waveEnemy_spawnCount, waveEnemy_index, waveEnemy_delay;

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

    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    int screenWidth;
    int screenHeight;

    Engine::ImageButton* Map_btn;
    bool isMap_clicked = false;

    void sensorAssign();

    //==================================
    Engine::Point lastPlayerPosition;

protected:
    int SpeedMult;
    float total_time;
    int total_money;
public:
    bool isCamLocked = false;

    enum TileType {
        TILE_AIR,
        TILE_DIRT,
        TILE_WPLATFORM,
    };
    std::unordered_map<Sensor*, std::vector<Door*>> DoorSensorAssignments;
    std::unordered_map<Door*, std::vector<Sensor*>> SensorDoorAssignments;
    static Engine::Point Camera;
    static float Gravity;
    static bool Pause;
    static const std::vector<Engine::Point> directions;
    static int MapWidth, MapHeight;
    static int BlockSize;
    static const float DangerTime;
    static Engine::Point SpawnGridPoint;
    static const std::vector<int> code;
    int MapId;
    float ticks = 0;
    float deathCountDown = 0;
    // Map tiles.
    Group *TileMapGroup;
    Group *GroundEffectGroup;
    Group *DebugIndicatorGroup;
    Group *BulletGroup;
    Group *EnemyBulletGroup;
    Group *PlayerGroup;
    Group *WeaponGroup;
    Group *EnemyGroup;
    Group *EffectGroup;
    Group *UIGroup;
    Group *DamageTextGroup;
    Group *InteractiveBlockGroup;
    Engine::Label *UIMoney;
    Engine::Label *UILives;
    Engine::Image *imgTarget;
    Engine::Sprite *dangerIndicator;
    std::vector<std::vector<TileType>> mapState;
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
    void EarnMoney(int m);
    void ReadMap();
    void ReadEnemyWave();
    void ConstructUI();
    void UIBtnClicked(int id);
    void HomeOnClick(int id);
    void MiniMap() const;
    void FullMap() const;
    void FlashLight() const;
    void MiniMapOnClick(int stage);

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
    Engine::ImageButton *enable2nd;

    Engine::Label *continueLabel;
    Engine::Label *exitLabel;
    Engine::Label *restartLabel;
    Engine::Label *BGMSlider;
    Engine::Label *SFXSlider;
    Engine::Label *enable2ndLabel;

    Slider *sliderBGM, *sliderSFX;
    //----
    void BackOnClick(int state);
    void ContinueOnClick(int state);
    void RestartOnClick(int state);
    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);

    void MazeCreator();

    Player *player1;
    Player *player2;
    bool enable2ndplayer = false;
    void Enable2ndPlayer(int stage);

    //dialog system
    enum class GameState {
        Normal,
        Dialog
    };
    GameState currentState = GameState::Normal;


    void StartDialog(const std::vector<Dialog>& dialogs, bool skippable);
    void UpdateDialog(float deltaTime);
    void RenderDialog() const;

    //remove player
    void RemovePlayer(Player *player);

    //for level 2 only
    int TotalMiniEjojo = 0;
    //for boss only
    int TotalArcherSkelly = 0;

    //for transition
    float transitionTick = 0.0f;
    float desiredTransitionTick = 2.0f;

    //========================SHOP=================================
    bool isShop = false;

    //
    ALLEGRO_BITMAP* mask;
};
#endif   // PLAYSCENE_HPP
