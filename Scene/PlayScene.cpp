#include <algorithm>
#include <allegro5/allegro.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <utility>
#include <random>
#include <ctime>
#include <algorithm>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"

#include <map>

#include "LeaderboardScene.hpp"

#include <allegro5/allegro_primitives.h>

#include "SelectProfileScene.h"
#include "Enemy/EjojoBoss.h"
#include "Enemy/ArcherSkelly.h"
#include "Enemy/EjojoEnemy.h"
#include "Player/MeleePlayer.hpp"
#include "Player/Player.h"
#include "Player/RangePlayer.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

#include "Enemy/Enemy.hpp"
#include "Enemy/SoldierEnemy.hpp"
#include "Enemy/FlyingEnemy.h"
#include "Enemy/FlyingDemon.h"
#include "Enemy/MiniEjojo.h"
#include "Enemy/SwordSkelly.h"
#include "InteractiveBlock/Box.h"
#include "InteractiveBlock/Buton.h"
#include "InteractiveBlock/Portal.h"
#include "InteractiveBlock/Door.h"
#include "InteractiveBlock/GasMachine.h"
#include "InteractiveBlock/Sensor.h"
#include "Player/JetpackPlayerA.h"
#include "Player/JetpackPlayerB.h"
#include "Player/MazePlayerA.h"
#include "Player/MazePlayerB.h"
#include "Player/TankPlayerA.h"
#include "Player/TankPlayerB.h"
#include "UI/Animation/DamageText.h"

namespace Engine {
    class ImageButton;
}

float PlayScene::total_time = 0.000f;
int PlayScene::MapWidth = 0, PlayScene::MapHeight = 0;
bool pressed;
Engine::Point PlayScene::Camera;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
float PlayScene::Gravity = 0;
const float PlayScene::DangerTime = 7.61;
Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const std::vector<int> PlayScene::code = {
    ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEY_LSHIFT, ALLEGRO_KEY_ENTER
};

Engine::Point PlayScene::GetClientSize() {
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}

void PlayScene::Initialize() {
    screenWidth = Engine::GameEngine::GetInstance().GetScreenWidth();
    screenHeight = Engine::GameEngine::GetInstance().GetScreenHeight();
    mask = nullptr;
    player1 = player2 = nullptr;

    isCamLocked = false;
    total_time = .0f;
    total_money = 0;

    Camera.x=0,Camera.y=0;
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    SpeedMult = 1;
    Gravity = 18.0f * BlockSize;

    //Pause
    IsPaused = false;
    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    AddNewObject(PlayerGroup = new Group());
    AddNewObject(WeaponGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    AddNewObject(BulletGroup = new Group());
    AddNewObject(EnemyBulletGroup = new Group());
    AddNewObject(DamageTextGroup = new Group());
    AddNewObject(InteractiveBlockGroup = new Group());
    AddNewObject(HealthBarGroup = new Group());
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    ReadMap();

    if (MapId == 5) {
        //for flashlight
        if (!mask) mask = al_create_bitmap(MapWidth * BlockSize,MapHeight * BlockSize);
    }
    else {
        if (mask) {
            al_destroy_bitmap(mask);
            mask = nullptr;
            al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
        }
    }

    if (MapId == 1 || MapId == 4) {
        ReadEnemyWave();
        waveEnemy_spawnCount = waveEnemy_index = waveEnemy_delay = -1;
    }
    ConstructUI();

    if (MapId == 1 || MapId == 2) {
        rainParticles.clear();
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distX(0, PlayScene::MapWidth * PlayScene::BlockSize);
        std::uniform_real_distribution<> distSpeed(5.0f, 15.0f);
        std::uniform_real_distribution<> distLength(10.0f, 30.0f);
        rainEnabled = true;


        for (int i = 0; i < count; ++i) {
            RainParticle p;
            p.x = distX(gen);
            p.y = static_cast<float>(distX(gen) % (PlayScene::MapHeight * PlayScene::BlockSize));
            p.speed = distSpeed(gen);
            p.length = distLength(gen);
            rainParticles.push_back(p);
        }
    }


    const float scale = 0.15f;
    const int border = BlockSize / 8;

    float mWidth = screenWidth * scale;;
    float mHeight = screenHeight * scale;
    float mX = screenWidth - mWidth - border;
    float mY = border;

    Map_btn = new Engine::ImageButton(
        "play/floor.png",
        "play/floor.png",
        mX,
        mY,
        mWidth,
        mHeight
    );

    Map_btn->Visible = false;
    Map_btn->SetOnClickCallback(std::bind(&PlayScene::MiniMapOnClick, this, 0));
    AddNewControlObject(Map_btn);


    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    UIGroup->AddNewObject(imgTarget);
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    if (AudioHelper::sharedBGMInstance) {
        AudioHelper::StopSample(AudioHelper::sharedBGMInstance);
        AudioHelper::sharedBGMInstance = nullptr;
    }
    bgmInstance = AudioHelper::PlaySample("play.ogg", true, AudioHelper::BGMVolume);

    CreatePauseUI();

    //dialogue=======================================================================
    dialogFont = al_load_font("Resource/fonts/pirulen.ttf", 36, 0);
    if (!dialogFont) {
        throw std::runtime_error("Failed to load font: Resource/fonts/pirulen.ttf");
    }
    std::vector<PlayScene::Dialog> dialogs;
    if (MapId == 1) {
        backgroundIMG = Engine::Resources::GetInstance().GetBitmap("play/background1.png");
        dialogs.push_back({
            "Monsters everywhere! This is insane!",
            3.0f,
            "play/arwenDialog.png",
            "Arwen"
        });
        dialogs.push_back({
            "Yeah, what the heck is going on? Skeletons?",
            3.0f,
            "play/bryanDialog.png",
            "Bryan"
        });
        dialogs.push_back({
            "No idea, but we need to deal with this!",
            3.0f,
            "play/arwenDialog.png",
            "Arwen"
        });
        dialogs.push_back({
            "Alright, let's take them down!",
            3.0f,
            "play/bryanDialog.png",
            "Bryan"
        });
        dialogs.push_back({
            "Controls: Arwen -> WASD, Bryan -> IJKL",
            4.0f,
            "play/Control.png",
            "Control"
        });
        dialogs.push_back({
            "Attack: Arwen -> C, Bryan -> N",
            4.0f,
            "play/Control.png",
            "Control"
        });
        dialogs.push_back({
            "Tip: Stay close to each other to survive!",
            4.0f,
            "play/tool-base.png",
            "SYSTEM"
        });
    }
    else if (MapId == 2) {
        backgroundIMG = Engine::Resources::GetInstance().GetBitmap("play/background1.png");
        dialogs.push_back({
            "The tank can only shoot upwards.",
            4.0f,
            "play/Control.png",
            "Control"
        });
        dialogs.push_back({
            "Same controls, but no jumping this time!",
            4.0f,
            "play/Control.png",
            "Control"
        });
        dialogs.push_back({
            "Good luck!",
            4.0f,
            "play/tool-base.png",
            "SYSTEM"
        });
    }

    else if (MapId == 3) {
        backgroundIMG = Engine::Resources::GetInstance().GetBitmap("play/shipbackground-3.png");
        dialogs.push_back({
            "Wow, this ship is enormous!",
            3.0f,
            "play/arwenDialog.png",
            "Arwen"
        });
        dialogs.push_back({
            "What's with all this advanced tech?",
            3.0f,
            "play/bryanDialog.png",
            "Bryan"
        });
        dialogs.push_back({
            "I have no idea.",
            3.0f,
            "play/arwenDialog.png",
            "Arwen"
        });
        dialogs.push_back({
            "Let's uncover the mystery behind it all.",
            3.0f,
            "play/bryanDialog.png",
            "Bryan"
        });
        dialogs.push_back({
            "Tip: Try interacting with any unusual objects.",
            3.0f,
            "play/tool-base.png",
            "SYSTEM"
        });
    }

    else if (MapId == 4) {
        dialogs.push_back({
            "The whole thing was confusing..",
            3.0f,
            "play/arwenDialog.png",
            "Arwen"
        });
        dialogs.push_back({
            "Don't worry you got genius beside you",
            3.0f,
            "play/bryanDialog.png",
            "Bryan"
        });
        dialogs.push_back({
            "Shut up DumbBoy",
            3.0f,
            "play/arwenDialog.png",
            "Arwen"
        });
        dialogs.push_back({
            "Anyway, what are we up to",
            3.0f,
            "play/bryanDialog.png",
            "Bryan"
        });
        dialogs.push_back({
            "Are you blind? look at those things over there",
            3.0f,
            "play/arwenDialog.png",
            "Arwen"
        });
        dialogs.push_back({
            "---/---",
            3.0f,
            "play/tool-base.png",
            "SYSTEM"
        });
    }
    else if (MapId == 5) {
        dialogs.push_back({
            "Fortunately we brought our own flashlight",
            3.0f,
            "play/ArwenDialog.png",
            "Arwen"
        });
        dialogs.push_back({
            "Can we both walk together? please... I don't like this",
            3.0f,
            "play/bryanDialog.png",
            "Bryan"
        });
        dialogs.push_back({
            "Hell no you coward..",
            3.0f,
            "play/ArwenDialog.png",
            "Arwen"
        });
        dialogs.push_back({
            "Fine...What are we even suppose to do anyway?",
            3.0f,
            "play/bryanDialog.png",
            "Bryan"
        });
        dialogs.push_back({
            "I don't know, just keep moving..",
            3.0f,
            "play/ArwenDialog.png",
            "Arwen"
        });
    }
    else if (MapId == 6) {
        dialogs.push_back({
            "Hello, adventurer!",
            3.0f,
            "play/arwen.png",
            "Arwen"
        });
        dialogs.push_back({
            "The castle is dangerous!",
            3.0f,
            "play/bryan.png",
            "Bryan"
        });
    }
    StartDialog(dialogs, true);
    //================================================================================
    //for transition
    transitionTick = 0.0f;

    //for specific
    TotalMiniEjojo = 0;
    TotalArcherSkelly = 0;

    if (MapId == 1 || MapId == 2 || MapId == 4 || MapId == 6) {
        // Player 1 health bar
        float padding = BlockSize /12;
        float headSize = BlockSize*0.8;
        float healthBarWidth = BlockSize * 3;
        float healthBarHeight = BlockSize/3;
        if (player1) {
            healthBarP1.head = new Engine::Image("play/bryanhead.png", 0, 0, headSize, headSize);
            healthBarP1.fg = new Engine::Image("play/healthbar_full.png", 0, 0, healthBarWidth, healthBarHeight);
            healthBarP1.bg = new Engine::Image("play/healthbar_empty.png", 0, 0, healthBarWidth, healthBarHeight);
            healthBarP1.player = player1;
            healthBarP1.isPlayer1 = true;

            HealthBarGroup->AddNewObject(healthBarP1.bg);
            HealthBarGroup->AddNewObject(healthBarP1.fg);
            HealthBarGroup->AddNewObject(healthBarP1.head);
        }

        // Player 2 health bar
        if (player2) {
            healthBarP2.head = new Engine::Image("play/arwenhead.png", 0, 0, headSize, headSize);
            healthBarP2.fg = new Engine::Image("play/healthbar_full.png", 0, 0, healthBarWidth, healthBarHeight);
            healthBarP2.bg = new Engine::Image("play/healthbar_empty.png", 0, 0, healthBarWidth, healthBarHeight);

            healthBarP2.player = player2;
            healthBarP2.isPlayer1 = false;

            HealthBarGroup->AddNewObject(healthBarP2.bg);
            HealthBarGroup->AddNewObject(healthBarP2.fg);
            HealthBarGroup->AddNewObject(healthBarP2.head);
        }

        // Initial positioning
        UpdateHealthBarPositions();
    }
}
void PlayScene::Terminate() {
    if (dialogFont) {
        al_destroy_font(dialogFont);
        dialogFont = nullptr;
    }

    player1 = player2 = nullptr;

    AudioHelper::StopSample(bgmInstance);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    rainParticles.clear();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {

    float clampedDT = std::min(deltaTime, Engine::GameEngine::GetInstance().GetDeltaTimeThreshold());
    total_time += clampedDT;

    if (MapId == 1) {
        if (EnemyGroup->GetObjects().empty() && enemyWave.empty()) {
            transitionTick += deltaTime;
            if (transitionTick >= desiredTransitionTick) {
                MapId++;

                //this 3 lines is for updating the profile.
                if (SelectProfileScene::isSaved && !SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].isWin) {
                    auto* newdata = new SelectProfileScene::textData();
                    newdata->level = MapId;
                    newdata->coin_counts = total_money;
                    SelectProfileScene::WriteProfileData(newdata);
                }
                //----------------------------------------

                if (!SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].isWin) Engine::GameEngine::GetInstance().ChangeScene("story");
                else Engine::GameEngine::GetInstance().ChangeScene("stage-select");

                return;
            }
        }
    }



    UpdatePauseState();
    if (IsPaused) {
        UIGroup->Update(deltaTime);
        return;
    }

    //DIALOG STATE+++++++++++++++++++++++++++++++++++++++++++
    if (currentState == GameState::Dialog) {
        UpdateDialog(deltaTime);
        UIGroup->Update(deltaTime);

        // Disable player movement but keep them visible
        for (auto& it : PlayerGroup->GetObjects()) {
            Player* player = dynamic_cast<Player*>(it);
            if (player) {
                player->EnableMovement(false);
            }
        }
    } else {
        // Enable player movement when not in dialog
        for (auto& it : PlayerGroup->GetObjects()) {
            Player* player = dynamic_cast<Player*>(it);
            if (player) {
                player->EnableMovement(true);
            }
        }
    }
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    for (auto& p : rainParticles) {
        p.y += p.speed * deltaTime * BlockSize; // Animates properly with deltaTime
        if (p.y > MapHeight * BlockSize * 2) {
            p.y = -p.length; // Reset above  the screen
            p.x = rand() % (PlayScene::MapWidth * PlayScene::BlockSize);
        }
    }
    if (MapId==1||MapId==2||MapId==4||MapId==6) {
        UpdateHealthBarPositions();
    }
    BulletGroup->Update(deltaTime);
    if (currentState != GameState::Dialog)EnemyBulletGroup->Update(deltaTime);
    WeaponGroup->Update(deltaTime);
    PlayerGroup->Update(deltaTime);
    if (currentState!=GameState::Dialog) EnemyGroup->Update(deltaTime);
    DamageTextGroup->Update(deltaTime);
    EffectGroup->Update(deltaTime);
    InteractiveBlockGroup->Update(deltaTime);

    //players
    std::vector<Player*> players;
    for (auto& it : PlayerGroup->GetObjects()) {
        Player *player = dynamic_cast<Player *>(it);
        if (player && player->Visible) {
            players.push_back(player);

            if (EnemyGroup->GetObjects().empty()) player->Tint = al_map_rgb(255,255,255);
        }
    }

    for (auto& it : players) {
        if (it->hp <= 0) {
            //PlayerGroup->RemoveObject(it->GetObjectIterator());
            if (SelectProfileScene::isSaved) {
                std::time_t now = std::time(nullptr);

                // Convert to local time
                std::tm tm{};

                #ifdef _WIN32
                    localtime_s(&tm, &now);  // Windows
                #else
                    localtime_r(&now, &tm);  // Linux / macOS
                #endif

                // Format date with slashes: YYYY/MM/DD
                std::ostringstream oss;
                oss << std::put_time(&tm, "%Y/%m/%d  %H:%M");
                std::string date_time = oss.str();

                SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Last_Played = date_time;
                SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Duration = std::to_string(std::stof(SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Duration) + total_time);
                SelectProfileScene::WriteProfileData(nullptr);
            }

            Engine::GameEngine::GetInstance().ChangeScene("lose");
        }
    }

    //damage text
    for (auto& obj : DamageTextGroup->GetObjects()) {
        DamageText* dt = dynamic_cast<DamageText*>(obj);
        if (dt && dt->removeDT) {
            DamageTextGroup->RemoveObject(dt->GetObjectIterator());
        }
    }

    float camLock_x;
    if (MapId == 1) camLock_x = MapWidth*BlockSize - BlockSize - screenWidth;
    else if (MapId == 4) camLock_x = BlockSize * 18;

    if (MapId == 1 && EnemyGroup->GetObjects().empty() && Camera.x >= camLock_x && !isCamLocked) {
        Camera.x = camLock_x;
        isCamLocked = true;
    } else if (MapId == 4 && EnemyGroup->GetObjects().empty() && Camera.x >= camLock_x && Camera.y <= BlockSize*8  && !isCamLocked) {
        isCamLocked = true;
    }

    if (isCamLocked && !enemyWave.empty()) {
        Enemy *enemy;
        if (waveEnemy_index == -1) waveEnemy_index = 0;

        int idx = waveEnemy_index;

        if (idx != -1 && idx < enemyWave.size() && ((waveEnemy_delay <= 0 && waveEnemy_spawnCount > 0) || waveEnemy_spawnCount == enemyWave[idx].count)) {
            waveEnemy_delay = enemyWave[idx].cooldown;
            waveEnemy_spawnCount--;

            float spawn_x = (enemyWave[idx].direction)? Camera.x + (-1.0f) * BlockSize : MapWidth * BlockSize - 1.0f * BlockSize;
            float spawn_y;
            if (MapId == 1) spawn_y = MapHeight * BlockSize - (enemyWave[idx].position_y) * BlockSize;
            else if (MapId == 4) spawn_y = (enemyWave[idx].position_y) * BlockSize;

            //std::cout << "Posisi SPAWN: " << enemyWave[idx].position_y << std::endl;

            switch (static_cast<int>(enemyWave[idx].type)) {
            case 1:
                EnemyGroup->AddNewObject(enemy = new SwordSkelly(spawn_x, spawn_y));
                break;
            case 2:
            case 3:
                EnemyGroup->AddNewObject(enemy = new SwordSkelly(spawn_x, spawn_y));
                break;
            default:
                break;
            }
        }
        else if (waveEnemy_spawnCount <= 0 && waveEnemy_index < static_cast<float>(enemyWave.size())) {
            waveEnemy_index++;
            waveEnemy_spawnCount = enemyWave[idx].count;
            waveEnemy_delay = enemyWave[idx].cooldown;
        }
        else if (waveEnemy_delay > 0) {
            waveEnemy_delay -= deltaTime;
        }
        else if (static_cast<int>(waveEnemy_index) == static_cast<int>(enemyWave.size())){
            enemyWave.clear();
            waveEnemy_spawnCount = waveEnemy_index = waveEnemy_delay = -1;
        }
    }

    if (players.size() == 2) {
        Engine::Point target = (players[0]->Position + players[1]->Position)/2;
        Camera.x += (target.x - screenWidth / 2 - Camera.x) * 0.1f;
        if (isCamLocked) Camera.x = camLock_x;

        Camera.y += (target.y - screenHeight / 2 - Camera.y) * 0.1f;
        // Camera.x = (target.x - screenWidth / 2);
        // Camera.y = (target.y - screenHeight / 2);
        if (Camera.x < 0)Camera.x = 0;
        if (Camera.x > MapWidth * BlockSize - screenWidth)Camera.x = MapWidth * BlockSize - screenWidth;
        if (Camera.y < 0)Camera.y = 0;
        if (Camera.y > MapHeight * BlockSize - screenHeight)Camera.y = MapHeight * BlockSize - screenHeight;
    }
    else if (players.size() == 1) {
        Engine::Point target = players[0]->Position;
        Camera.x = (target.x - screenWidth / 2);
        if (isCamLocked) Camera.x = camLock_x;

        Camera.y = (target.y - screenHeight / 2);
        if (Camera.x < 0)Camera.x = 0;
        if (Camera.x > MapWidth * BlockSize - screenWidth)Camera.x = MapWidth * BlockSize - screenWidth;
        if (Camera.y < 0)Camera.y = 0;
        if (Camera.y > MapHeight * BlockSize - screenHeight)Camera.y = MapHeight * BlockSize - screenHeight;
        lastPlayerPosition = target;
    }
    else {
        Camera.x = lastPlayerPosition.x - screenWidth / 2;
        Camera.y = lastPlayerPosition.y - screenHeight / 2;
        if (Camera.x < 0)Camera.x = 0;
        if (Camera.x > MapWidth * BlockSize - screenWidth)Camera.x = MapWidth * BlockSize - screenWidth;
        if (Camera.y < 0)Camera.y = 0;
        if (Camera.y > MapHeight * BlockSize - screenHeight)Camera.y = MapHeight * BlockSize - screenHeight;
        if (MapId == 3 || MapId == 4 || MapId == 5) {
            transitionTick += deltaTime;
            if (transitionTick >= desiredTransitionTick) {
                MapId++;

                //this 3 lines is for updating the profile.
                if (SelectProfileScene::isSaved && !SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].isWin) {
                    auto* newdata = new SelectProfileScene::textData();
                    newdata->level = MapId;
                    newdata->coin_counts = total_money;
                    SelectProfileScene::WriteProfileData(newdata);
                }
                else {

                }
                //----------------------------------------

                if (!SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].isWin) Engine::GameEngine::GetInstance().ChangeScene("story");
                else Engine::GameEngine::GetInstance().ChangeScene("stage-select");
            }
        }
    }
    float healthBarWidth = BlockSize * 3; // Match initialization width

    // Player 1 health bar update
    if (player1 && healthBarP1.fg) {
        float healthRatio = player1->hp / player1->MaxHp;
        healthRatio = std::max(0.0f, std::min(1.0f, healthRatio)); // Clamp between 0-1
        healthBarP1.fg->Size.x = healthBarWidth * healthRatio;

        // Update visibility based on player state
        bool shouldShow = player1->Visible && player1->hp > 0;
        healthBarP1.head->Visible = shouldShow;
        healthBarP1.bg->Visible = shouldShow;
        healthBarP1.fg->Visible = shouldShow;
    }

    // Player 2 health bar update
    if (player2 && healthBarP2.fg) {
        float healthRatio = player2->hp / player2->MaxHp;
        healthRatio = std::max(0.0f, std::min(1.0f, healthRatio)); // Clamp between 0-1
        healthBarP2.fg->Size.x = healthBarWidth * healthRatio;

        // Update visibility based on player state
        bool shouldShow = player2->Visible && player2->hp > 0;
        healthBarP2.head->Visible = shouldShow;
        healthBarP2.bg->Visible = shouldShow;
        healthBarP2.fg->Visible = shouldShow;
    }
}
void PlayScene::Draw() const {
    if (IsPaused) {
        UIGroup->Draw();
        return;
    }

    ALLEGRO_TRANSFORM trans;
    al_identity_transform(&trans);
    al_translate_transform(&trans, -Camera.x, -Camera.y);  // apply camera offset
    al_use_transform(&trans);  // set transform for all following drawing

    IScene::Draw(); // will draw tiles/UI, now offset by camera

    if (MapId == 5 && !DebugMode) FlashLight();

    PlayerGroup->Draw();
    WeaponGroup->Draw();
    EffectGroup->Draw();
    EnemyBulletGroup->Draw();
    HealthBarGroup->Draw();

    al_identity_transform(&trans);
    al_use_transform(&trans);
    // Draw UI elements (health bars, minimap, etc.)
    //for map debug
    if((MapId == 5 && DebugMode) || (MapId != 5)) {
        Map_btn->Enabled = true;
        if (isMap_clicked) FullMap();
        else MiniMap();
    }

    if (currentState == GameState::Dialog) {
        RenderDialog();
    }

    //for transition
    if (transitionTick > 0) {
        float alpha = std::min(transitionTick / desiredTransitionTick, 1.0f); // from 0.0 to 1.0
        int alpha255 = static_cast<int>(alpha * 255);
        ALLEGRO_COLOR fadeColor;

        if (transitionTick >= desiredTransitionTick) {
            fadeColor = al_map_rgba(0, 0, 0, 255);
        } else {
            fadeColor = al_map_rgba(0, 0, 0, alpha255);
        }

        al_draw_filled_rectangle(0, 0,
            Engine::GameEngine::GetInstance().GetScreenWidth(),
            Engine::GameEngine::GetInstance().GetScreenHeight(),
            fadeColor);
    }


    if (DebugMode) {
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                float x = j * BlockSize - Camera.x;
                float y = i * BlockSize - Camera.y;
                ALLEGRO_COLOR color = al_map_rgba(255, 255, 255, 100);
                if (mapState[i][j] == TILE_DIRT) {
                    color = al_map_rgb(155, 0, 0);
                }
                else if (mapState[i][j] == TILE_WPLATFORM) {
                    color = al_map_rgb(0, 255, 0);
                }
                al_draw_rectangle(x, y, x + BlockSize, y + BlockSize, color, 2.0);

            }
        }
    }

}
void PlayScene::OnMouseDown(int button, int mx, int my) {
    IScene::OnMouseDown(button, mx, my);
    if (currentState == GameState::Dialog && dialogSkippable) {
        dialogSkipRequested = true;
    }
}
void PlayScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
}
void PlayScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);

    if (IsPaused && keyCode != ALLEGRO_KEY_ESCAPE) return;

    if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
        // Hotkey for Speed up.
        SpeedMult = keyCode - ALLEGRO_KEY_0;
    }
    if (keyCode == ALLEGRO_KEY_ESCAPE) {
        IsPaused = !IsPaused;
    }

    if (currentState == GameState::Dialog && dialogSkippable) {
        if (keyCode == ALLEGRO_KEY_SPACE || keyCode == ALLEGRO_KEY_ENTER) {
            dialogSkipRequested = true;
        }
    }
}

void PlayScene::EarnMoney(int m) {
    total_money += m;
}

void PlayScene::ReadMap() {
    if (MapId==5)MazeCreator();
    std::string filename = std::string("Resource/map") + std::to_string(MapId) + ".txt";
    // Read map file.
    char c;
    std::vector<char> mapData;
    std::ifstream fin(filename);
    fin >> MapHeight >> MapWidth;
    fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    while (fin >> c) {
        switch (c) {
            case '0': mapData.push_back('0'); break;
            case '1': mapData.push_back('1'); break;
            case '2': mapData.push_back('2'); break;
            case 'E': mapData.push_back('E'); break;
            case 'B': mapData.push_back('B'); break;
            case 'A': mapData.push_back('A'); break;
            case 'X': mapData.push_back('X'); break;
            case 'G': mapData.push_back('G'); break;
            case 'R': mapData.push_back('R'); break;
            case 'M': mapData.push_back('M'); break;
            case 'F': mapData.push_back('F'); break;
            case 'S': mapData.push_back('S'); break;
            case 'N': mapData.push_back('N'); break;
            case 'D': mapData.push_back('D'); break;
            case 'T': mapData.push_back('T'); break;
            case '3': mapData.push_back('3'); break;
            case '4': mapData.push_back('4'); break;
            case '5': mapData.push_back('5'); break;
            case '6': mapData.push_back('6'); break;
            case 'L': mapData.push_back('L'); break;
            case 's': mapData.push_back('s'); break;
            case 'a': mapData.push_back('a'); break;
            case '<': mapData.push_back('<'); break;
            case 'g': mapData.push_back('g'); break;
            case '>': mapData.push_back('>'); break;
            case '8': mapData.push_back('8'); break;
            case '9': mapData.push_back('9'); break;
            case '7': mapData.push_back('7'); break;
            case '\n':
            case '\r':
                if (static_cast<int>(mapData.size()) / MapWidth != 0)
                    throw std::ios_base::failure("Map data is corrupted 1.");
                break;
            default: throw std::ios_base::failure("Map data is corrupted 2.");
        }
    }
    fin.close();
    // Validate map data.
    if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
        throw std::ios_base::failure("Map data is corrupted 3.");
    // Store map in 2d array.
    mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            int idx = i * MapWidth + j;
            const char num = mapData[idx];
            switch (num) {
                case '0':
                    mapState[i][j]=TILE_AIR;
                    break;
                case '1':
                    mapState[i][j]=TILE_DIRT;
                    break;
                case '2':
                    mapState[i][j]=TILE_WPLATFORM;
                    break;
                case '7':
                    mapState[i][j]=TILE_WPLATFORM;
                    break;
                case 'B':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'A':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'E':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'X':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'G':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'R':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'M':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'F':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'S':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'N':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'T':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'D':
                    mapState[i][j]=TILE_AIR;
                    break;
                case '3':
                    mapState[i][j]=TILE_AIR;
                    break;
                case '4':
                    mapState[i][j]=TILE_DIRT;
                    break;
                case '5':
                    mapState[i][j]=TILE_AIR;
                    break;
                case '6':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'L':
                    mapState[i][j]=TILE_AIR;
                case 's':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'a':
                    mapState[i][j]=TILE_AIR;
                    break;
                case '<':
                    mapState[i][j]=TILE_AIR;
                    break;
                case '>':
                    mapState[i][j]=TILE_AIR;
                    break;
                case 'g':
                    mapState[i][j]=TILE_AIR;
                    break;
                case '8':
                    mapState[i][j]=TILE_AIR;
                    break;
                case '9':
                    mapState[i][j]=TILE_DIRT;
                    break;

                default:
                    mapState[i][j]=TILE_AIR;
                    break;

            }
            if (num=='0')
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            else if (num=='1') {
                TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] != '1')
                    TileMapGroup->AddNewObject(new Engine::Image("play/grass.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if((mapData[idx-1])!='1'&& idx%MapWidth!=0)
                    TileMapGroup->AddNewObject(new Engine::Image("play/lborder.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx+1] != '1'&& idx%MapWidth!=(MapWidth-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/rborder.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if((mapData[idx-MapWidth-1])!='1'&& idx%MapWidth!=0 && mapData[idx-MapWidth] == '1' && mapData[idx-1] == '1')
                    TileMapGroup->AddNewObject(new Engine::Image("play/tlborder.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth+1] != '1'&& idx%MapWidth!=(MapWidth-1) && mapData[idx-MapWidth] == '1' && mapData[idx+1] == '1')
                    TileMapGroup->AddNewObject(new Engine::Image("play/trborder.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }
            else if (num=='2') {
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                TileMapGroup->AddNewObject(new Engine::Image("play/platform.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // if ((mapData[idx-1]=='1'&&mapData[idx+1]=='1')&&idx%MapWidth!=(MapWidth-1)&&idx%MapWidth!=0)
                //     TileMapGroup->AddNewObject(new Engine::Image("play/platform-4.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // else if (mapData[idx-1]=='1')
                //     TileMapGroup->AddNewObject(new Engine::Image("play/platform-2.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // else if (mapData[idx+1]=='1')
                //     TileMapGroup->AddNewObject(new Engine::Image("play/platform-3.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // else if (mapData[idx-1]=='2'||mapData[idx+1]=='2')
                //     TileMapGroup->AddNewObject(new Engine::Image("play/platform-1.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }
            else if (num=='7') {
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize/6));
                TileMapGroup->AddNewObject(new Engine::Image("play/platform-steel.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize/6));
            }
            else if (num == 'B') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                player1 = new MeleePlayer(SpawnCoordinate.x, SpawnCoordinate.y);
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                PlayerGroup->AddNewObject(player1);
            }
            else if (num == 'A') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                player2 = (new RangePlayer(SpawnCoordinate.x, SpawnCoordinate.y));
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                PlayerGroup->AddNewObject(player2);
            }
            else if (num == 'E') {
                Engine::Point EnemySpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                EnemyGroup->AddNewObject(new SoldierEnemy(EnemySpawnCoordinate.x, EnemySpawnCoordinate.y));
            }
            else if (num == 'X') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                InteractiveBlockGroup->AddNewObject(new Box("play/box.png",SpawnCoordinate.x, SpawnCoordinate.y));
            }else if (num == 'G') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                player1 = (new TankPlayerB(SpawnCoordinate.x, SpawnCoordinate.y));
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                PlayerGroup->AddNewObject(player1);
            }else if (num == 'R') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                player2 = (new TankPlayerA(SpawnCoordinate.x, SpawnCoordinate.y));
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                PlayerGroup->AddNewObject(player2);
            }else if (num == 'M') {
                Engine::Point EnemySpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                EnemyGroup->AddNewObject(new EjojoEnemy("play/ejojo.png",EnemySpawnCoordinate.x, EnemySpawnCoordinate.y));
            }else if (num == 'F') {
                Engine::Point EnemySpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                EnemyGroup->AddNewObject(new FlyingDemon(EnemySpawnCoordinate.x, EnemySpawnCoordinate.y));
            }else if (num == 'S') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                InteractiveBlockGroup->AddNewObject(new Sensor("play/sensor.png",SpawnCoordinate.x, SpawnCoordinate.y));
            }else if (num == 'D') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize + BlockSize/2);
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-1]=='0') {
                    InteractiveBlockGroup->AddNewObject(new Door("play/dor.png",SpawnCoordinate.x, SpawnCoordinate.y, Door::CLOSE,false));
                }else if (mapData[idx-MapWidth]=='0') {
                    InteractiveBlockGroup->AddNewObject(new Door("play/dor1.png",SpawnCoordinate.x, SpawnCoordinate.y, Door::CLOSE,true));
                }
            }else if (num == 'N') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                player2 = (new MazePlayerA(SpawnCoordinate.x, SpawnCoordinate.y));
                TileMapGroup->AddNewObject(new Engine::Image("play/floortile.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                PlayerGroup->AddNewObject(player2);
            }
            else if (num == 'T') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                player1 = (new MazePlayerB(SpawnCoordinate.x, SpawnCoordinate.y));
                TileMapGroup->AddNewObject(new Engine::Image("play/floortile.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                PlayerGroup->AddNewObject(player1);
            } else if (num=='3'){
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            } else if (num=='4') {
                TileMapGroup->AddNewObject(new Engine::Image("play/blocktile.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }else if (num=='5'){
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize + BlockSize/2);
                TileMapGroup->AddNewObject(new Engine::Image("play/floortile.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                InteractiveBlockGroup->AddNewObject(new Buton("play/button.png",SpawnCoordinate.x, SpawnCoordinate.y));
            } else if (num=='6') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize + BlockSize/2);
                TileMapGroup->AddNewObject(new Engine::Image(MapId==5?"play/floortile.png":"play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                InteractiveBlockGroup->AddNewObject(new Portal("play/portal.png",SpawnCoordinate.x, SpawnCoordinate.y));
            } else if (num == 'L') {
                Engine::Point EnemySpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                EnemyGroup->AddNewObject(new EjojoBoss("play/EjojoBoss.png",EnemySpawnCoordinate.x, EnemySpawnCoordinate.y));
            }
            else if (num == 's') {
                Engine::Point EnemySpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                EnemyGroup->AddNewObject(new SwordSkelly(EnemySpawnCoordinate.x, EnemySpawnCoordinate.y));
            }
            else if (num == 'a') {
                Engine::Point EnemySpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                EnemyGroup->AddNewObject(new ArcherSkelly(EnemySpawnCoordinate.x, EnemySpawnCoordinate.y));
            }
            else if (num == 'g') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                InteractiveBlockGroup->AddNewObject(new GasMachine("play/gaschamber.png",SpawnCoordinate.x, SpawnCoordinate.y));
            }
            else if (num == '<') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                player2 = (new JetpackPlayerA(SpawnCoordinate.x, SpawnCoordinate.y));
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                PlayerGroup->AddNewObject(player2);
            }
            else if (num == '>') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                player1 = (new JetpackPlayerB(SpawnCoordinate.x, SpawnCoordinate.y));
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                PlayerGroup->AddNewObject(player1);
            } else if (num=='8'){
                TileMapGroup->AddNewObject(new Engine::Image("play/floortile.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }else if (num == '9') {
                if (mapData[idx-MapWidth] != '9' && mapData[idx-1] != '9' && mapData[idx+1] != '9' && mapData[idx+MapWidth] == '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-1.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] == '9' && mapData[idx-1] != '9' && mapData[idx+1] != '9' && mapData[idx+MapWidth] != '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-3.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] == '9' && mapData[idx+MapWidth] == '9' && mapData[idx-1]!='9' && mapData[idx+1]!='9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-2.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] == '9' && mapData[idx-1] != '9' && mapData[idx+1] == '9' && mapData[idx+MapWidth] != '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-4.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] != '9' && mapData[idx-1] == '9' && mapData[idx+1] == '9' && mapData[idx+MapWidth] != '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-5.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] == '9' && mapData[idx-1] == '9' && mapData[idx+1] != '9' && mapData[idx+MapWidth] != '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-6.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] != '9' && mapData[idx-1] == '9' && mapData[idx+1] != '9' && mapData[idx+MapWidth] == '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-15.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] != '9' && mapData[idx-1] != '9' && mapData[idx+1] == '9' && mapData[idx+MapWidth] == '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-16.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] != '9' && mapData[idx-1] == '9' && mapData[idx+1] == '9' && mapData[idx+MapWidth] == '9')
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-17.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] == '9' && mapData[idx-1] == '9' && mapData[idx+1] == '9' && mapData[idx+MapWidth] != '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-9.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] != '9' && mapData[idx-1] != '9' && mapData[idx+1] == '9' && mapData[idx+MapWidth] != '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-19.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] != '9' && mapData[idx-1] == '9' && mapData[idx+1] != '9' && mapData[idx+MapWidth] != '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-20.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] == '9' && mapData[idx-1] == '9' && mapData[idx+1] == '9' && mapData[idx+MapWidth] == '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-18.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] == '9' && mapData[idx-1] == '9' && mapData[idx+1] != '9' && mapData[idx+MapWidth] == '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-21.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] == '9' && mapData[idx-1] != '9' && mapData[idx+1] == '9' && mapData[idx+MapWidth] == '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-22.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] != '9' && mapData[idx-1] != '9' && mapData[idx+1] != '9' && mapData[idx+MapWidth] != '9'
                    && idx%MapWidth!=0 && idx%MapWidth!=MapWidth-1 && idx>MapWidth && idx<MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-23.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));

                if (idx==0)
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-7.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (idx==MapWidth)
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-8.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (idx<MapWidth && idx!=0 && mapData[idx+MapWidth]!='9')
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-5.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (idx%MapWidth == 0 && idx!= 0 && mapData[idx+1]!='9')
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-10.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (idx%MapWidth == MapWidth-1 && mapData[idx-1]!='9')
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-11.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (idx%MapWidth == 0 && idx!= 0 && mapData[idx+1]=='9')
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-25.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (idx%MapWidth == MapWidth-1 && mapData[idx-1]=='9')
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-24.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));

                if (idx>MapWidth*(MapHeight-1) && idx!=MapWidth*MapHeight)
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-12.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (idx==MapWidth*MapHeight)
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-13.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (idx==MapWidth*(MapHeight-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/wall-14.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }
        }
    }
    if (MapId == 3) {
        DoorSensorAssignments.clear();
        sensorAssign();
    }

    // SelectProfileScene::textData* savedData = SelectProfileScene::ReadProfileData(SelectProfileScene::getProfileID());
    // if (savedData) {
    //     player1->hp = savedData->HP_1;
    //     player2->hp = savedData->HP_2;
    // }
}

void PlayScene::sensorAssign() {
    std::vector<Door*> door_address;
    std::vector<Sensor*> sensor_address;

    for (auto it : InteractiveBlockGroup->GetObjects()) {
        Door* doorIt = dynamic_cast<Door*>(it);
        Sensor* sensorIt = dynamic_cast<Sensor*>(it);

        if (doorIt) {
            door_address.push_back(doorIt);
            //doorIt->Tint = al_map_rgb(0,0,0);
        }

        if (sensorIt) {
            sensor_address.push_back(sensorIt);
            //sensorIt->Tint = al_map_rgb(0,0,0);
        }
    }

    std::string filename = "Resource/sensor_map" + std::to_string(MapId) + ".txt";
    std::ifstream ifs(filename, std::ios_base::in);
    if (ifs.is_open()) {
        int idx = 0;
        std::string line;
        while (getline(ifs, line, '\n')) {
            std::vector<Door*> doorList;
            int mass = -1;

            std::istringstream iss(line);
            std::string token;

            while (iss >> token) {
                // Check if token is a number
                if (!token.empty() && std::isdigit(token[0])) {
                    // Convert to integer
                    int value = std::stoi(token);

                    if (mass == -1) {
                        mass = value;
                    } else {
                        Door* door = door_address[value];
                        doorList.push_back(door);
                        SensorDoorAssignments[door].push_back(sensor_address[idx]);
                    }
                }
            }

            sensor_address[idx]->Weight = mass;
            DoorSensorAssignments.insert({sensor_address[idx], doorList});
            idx++;
        }

        for (auto it : sensor_address) {
            std::cout << "DOOR ADDRESS: ";
            for (auto d : DoorSensorAssignments[it]) {
                std::cout << d << " ";
            }
            std::cout << std::endl;
        }

        door_address.clear();
        sensor_address.clear();

        ifs.close();
    } else {
        std::cerr << "[ERROR] Could not open ButtonAssignment.txt for reading!" << std::endl;
    }
}


void PlayScene::MiniMap() const {
    // Fixed minimap size (15% of screen)
    const float scale = 0.15f;
    const int border = BlockSize / 8;
    const float miniWidth = screenWidth * scale;
    const float miniHeight = screenHeight * scale;
    const float miniX = screenWidth - miniWidth - border;
    const float miniY = border;

    // Colors
    const ALLEGRO_COLOR bgColor = al_map_rgba(0, 0, 0, 200);
    const ALLEGRO_COLOR enemy_color = al_map_rgb(255, 0, 0);
    const ALLEGRO_COLOR p2_color = al_map_rgb(120, 0, 255);
    const ALLEGRO_COLOR p1_color = al_map_rgb(0, 255, 0);
    const ALLEGRO_COLOR tile_color = al_map_rgb(100, 100, 100);

    al_draw_filled_rounded_rectangle(miniX, miniY, miniX + miniWidth, miniY + miniHeight, 5, 5, bgColor);

    // Set clipping to restrict all further drawing to the minimap bounds
    al_set_clipping_rectangle(miniX, miniY, miniWidth, miniHeight);

    // Calculate scaling (screen to minimap)
    const float xScale = miniWidth / screenWidth;
    const float yScale = miniHeight / screenHeight;

    // Calculate visible area
    const int start_x = std::max(0, static_cast<int>(Camera.x / BlockSize));
    const int end_x = std::min(MapWidth, static_cast<int>((Camera.x + screenWidth) / BlockSize + 1));
    const int start_y = std::max(0, static_cast<int>(Camera.y / BlockSize));
    const int end_y = std::min(MapHeight, static_cast<int>((Camera.y + screenHeight) / BlockSize + 1));

    // Draw visible tiles
    for (int y = start_y; y < end_y; y++) {
        for (int x = start_x; x < end_x; x++) {
            if (mapState[y][x] == TILE_WPLATFORM) {
                float mx = miniX + (x * BlockSize - Camera.x) * xScale;
                float my = miniY + (y * BlockSize - Camera.y) * yScale;
                float mw = BlockSize * xScale;
                float mh = BlockSize * yScale;
                al_draw_line(mx, my, mx + mw, my, tile_color, 2.0f);
            }
            else if (mapState[y][x] != TILE_AIR) {
                float mx = miniX + (x * BlockSize - Camera.x) * xScale;
                float my = miniY + (y * BlockSize - Camera.y) * yScale;
                float mw = BlockSize * xScale;
                float mh = BlockSize * yScale;

                al_draw_filled_rectangle(mx, my, mx + mw, my + mh, tile_color);
            }
        }
    }

    // Draw players
    for (auto& obj : PlayerGroup->GetObjects()) {
        Player* player = dynamic_cast<Player*>(obj);
        if (player && player->Visible) {
            float px = miniX + (player->Position.x - Camera.x) * xScale;
            float py = miniY + (player->Position.y - Camera.y) * yScale;
            float radius = 16.0f * scale;

            al_draw_filled_circle(px, py, radius, (player == player1) ? p1_color : p2_color);
        }
    }

    for (auto& obj : EnemyGroup->GetObjects()) {
        Enemy *enemy = dynamic_cast<Enemy *>(obj);
        if (enemy && enemy->Visible && enemy->getHp() > 0) {
            float px = miniX + (enemy->Position.x - Camera.x) * xScale;
            float py = miniY + (enemy->Position.y - Camera.y) * yScale;
            float radius = 16.0f * scale;

            al_draw_filled_circle(px, py, radius, enemy_color);
        }
    }

    for (auto& obj : InteractiveBlockGroup->GetObjects()) {
        auto* sensor = dynamic_cast<Sensor*>(obj);
        auto* box = dynamic_cast<Box*>(obj);
        auto* buton = dynamic_cast<Buton*>(obj);
        auto* portal = dynamic_cast<Portal*>(obj);
        auto* door = dynamic_cast<Door*>(obj);

        if (sensor && sensor->Visible) {
            auto bmp = sensor->Bitmap.get();
            float px = miniX + (sensor->Position.x - Camera.x) * xScale;
            float py = miniY + (sensor->Position.y - Camera.y) * yScale;
            float sx = sensor->Size.x * xScale;
            float sy = sensor->Size.y * yScale;
            px -= sx / 2;
            py += sy;
            if (bmp) {
                al_draw_scaled_bitmap(
                bmp,
                0, 0,
                al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
                px, py,
                sx, sy,
                0
                );
            }
        }
        if (box && box->Visible) {
            auto bmp = box->Bitmap.get();
            float px = miniX + (box->Position.x - Camera.x) * xScale;
            float py = miniY + (box->Position.y - Camera.y) * yScale;
            float sx = box->Size.x * xScale;
            float sy = box->Size.y * yScale;
            px -= sx / 2;
            py -= sy / 2;
            if (bmp) {
                al_draw_scaled_bitmap(
                bmp,
                0, 0,
                al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
                px, py,
                sx, sy,
                0
                );
            }
        }
        if (buton && buton->Visible) {
            auto bmp = buton->Bitmap.get();
            float px = miniX + (buton->Position.x - Camera.x) * xScale;
            float py = miniY + (buton->Position.y - Camera.y) * yScale;
            float sx = buton->Size.x * xScale;
            float sy = buton->Size.y * yScale;
            px -= sx / 2;
            py -= sy / 2;
            if (bmp) {
                al_draw_scaled_bitmap(
                bmp,
                0, 0,
                al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
                px, py,
                sx, sy,
                0
                );
            }
        }
        if (portal && portal->Visible) {
            auto bmp = portal->Bitmap.get();
            float px = miniX + (portal->Position.x - Camera.x) * xScale;
            float py = miniY + (portal->Position.y - Camera.y) * yScale;
            float sx = portal->Size.x * xScale;
            float sy = portal->Size.y * yScale;
            px -= sx / 2;
            py += sy;
            if (bmp) {
                al_draw_scaled_bitmap(
                bmp,
                0, 0,
                al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
                px, py,
                sx, sy,
                0
                );
            }
        }
        if (door && door->Visible) {
            auto bmp = door->Bitmap.get();
            float px = miniX + (door->Position.x - Camera.x) * xScale;
            float py = miniY + (door->Position.y - Camera.y) * yScale;
            float sx = door->Size.x * xScale;
            float sy = door->Size.y * yScale;
            px -= sx / 2;
            py -= sy / 2;
            if (bmp) {
                al_draw_scaled_bitmap(
                bmp,
                0, 0,
                al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
                px, py,
                sx, sy,
                0
                );
            }
        }
    }

    // Reset clipping to full screen
    al_set_clipping_rectangle(0, 0, screenWidth, screenHeight);

    // Border on top (over the clipped content)
    al_draw_rounded_rectangle(miniX, miniY, miniX + miniWidth, miniY + miniHeight, 5, 5, al_map_rgb(255, 255, 255), 3.0f);Engine::ImageButton *btn;

    Map_btn->Position.x = miniX;
    Map_btn->Position.y = miniY;
    Map_btn->Size.x = miniX + miniWidth;
    Map_btn->Size.y = miniY + miniHeight;
}

void PlayScene::FullMap() const {
    // Calculate map dimensions in pixels
    const float mapPixelWidth = MapWidth * BlockSize;
    const float mapPixelHeight = MapHeight * BlockSize;

    // Determine maximum size that fits within 75% of screen while maintaining aspect ratio
    const float maxWidth = screenWidth * 0.75f;
    const float maxHeight = screenHeight * 0.75f;

    // Calculate scale factor while maintaining aspect ratio
    float scale = std::min(maxWidth / mapPixelWidth, maxHeight / mapPixelHeight);
    float scaledWidth = mapPixelWidth * scale;
    float scaledHeight = mapPixelHeight * scale;

    // Center the map on screen
    const float mapX = (screenWidth - scaledWidth) / 2;
    const float mapY = (screenHeight - scaledHeight) / 2;
    const int border = BlockSize / 8;

    // Colors
    const ALLEGRO_COLOR bgColor = al_map_rgba(0, 0, 0, 200);
    const ALLEGRO_COLOR enemy_color = al_map_rgb(255, 0, 0);
    const ALLEGRO_COLOR p2_color = al_map_rgb(120, 0, 255);
    const ALLEGRO_COLOR p1_color = al_map_rgb(0, 255, 0);
    const ALLEGRO_COLOR tile_color = al_map_rgb(100, 100, 100);

    // Draw background
    al_draw_filled_rounded_rectangle(mapX, mapY, mapX + scaledWidth, mapY + scaledHeight, 5, 5, bgColor);

    // Set clipping to restrict drawing to the map area
    al_set_clipping_rectangle(mapX, mapY, scaledWidth, scaledHeight);

    // Draw all tiles (entire map)
    for (int y = 0; y < MapHeight; y++) {
        for (int x = 0; x < MapWidth; x++) {
            if (mapState[y][x] == TILE_WPLATFORM) {
                float fx = mapX + x * BlockSize * scale;
                float fy = mapY + y * BlockSize * scale;
                float fw = BlockSize * scale;
                float fh = BlockSize * scale;
                al_draw_line(fx, fy, fx + fw, fy, tile_color, 4.0f);
            }
            else if (mapState[y][x] != TILE_AIR) {
                float fx = mapX + x * BlockSize * scale;
                float fy = mapY + y * BlockSize * scale;
                float fw = BlockSize * scale;
                float fh = BlockSize * scale;

                al_draw_filled_rectangle(fx, fy, fx + fw, fy + fh, tile_color);
            }
        }
    }

    // Draw players
    for (auto& obj : PlayerGroup->GetObjects()) {
        Player* player = dynamic_cast<Player*>(obj);
        if (player && player->Visible) {
            float px = mapX + player->Position.x * scale;
            float py = mapY + player->Position.y * scale;
            float radius = 16.0f * scale;

            al_draw_filled_circle(px, py, radius, (player == player1) ? p1_color : p2_color);
        }
    }

    for (auto& obj : EnemyGroup->GetObjects()) {
        Enemy *enemy = dynamic_cast<Enemy *>(obj);
        if (enemy && enemy->Visible) {
            float px = mapX + (enemy->Position.x) * scale;
            float py = mapY + (enemy->Position.y) * scale;
            float radius = 16.0f * scale;

            al_draw_filled_circle(px, py, radius, enemy_color);
        }
    }

    // Draw interactive objects
    for (auto& obj : InteractiveBlockGroup->GetObjects()) {
        auto* sensor = dynamic_cast<Sensor*>(obj);
        auto* box = dynamic_cast<Box*>(obj);
        auto* buton = dynamic_cast<Buton*>(obj);
        auto* portal = dynamic_cast<Portal*>(obj);
        auto* door = dynamic_cast<Door*>(obj);

        // if (sensor && sensor->Visible) {
        //     auto bmp = sensor->Bitmap.get();
        //     if (bmp) {
        //         float px = mapX + (sensor->Position.x - sensor->Size.x / 2) * scale;
        //         float py = mapY + sensor->Position.y * scale;
        //         float sx = sensor->Size.x * scale;
        //         float sy = sensor->Size.y * scale;
        //
        //         al_draw_scaled_bitmap(
        //             bmp,
        //             0, 0,
        //             al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
        //             px, py,
        //             sx, sy,
        //             0
        //         );
        //     }
        // }
        //
        // if (box && box->Visible) {
        //     auto bmp = box->Bitmap.get();
        //     if (bmp) {
        //         float px = mapX + (box->Position.x - box->Size.x / 2) * scale;
        //         float py = mapY + (box->Position.y - box->Size.y / 2) * scale;
        //         float sx = box->Size.x * scale;
        //         float sy = box->Size.y * scale;
        //
        //         al_draw_scaled_bitmap(
        //             bmp,
        //             0, 0,
        //             al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
        //             px, py,
        //             sx, sy,
        //             0
        //         );
        //     }
        // }

        if (buton && buton->Visible) {
            auto bmp = buton->Bitmap.get();
            if (bmp) {
                float px = mapX + (buton->Position.x - buton->Size.x / 2) * scale;
                float py = mapY + (buton->Position.y - buton->Size.y / 2) * scale;
                float sx = buton->Size.x * scale;
                float sy = buton->Size.y * scale;

                al_draw_scaled_bitmap(
                    bmp,
                    0, 0,
                    al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
                    px, py,
                    sx, sy,
                    0
                );
            }
        }
        if (portal && portal->Visible) {
            auto bmp = portal->Bitmap.get();
            if (bmp) {
                float px = mapX + (portal->Position.x - portal->Size.x / 2) * scale;
                float py = mapY + (portal->Position.y - portal->Size.y / 2) * scale;
                float sx = portal->Size.x * scale;
                float sy = portal->Size.y * scale;

                al_draw_scaled_bitmap(
                    bmp,
                    0, 0,
                    al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
                    px, py,
                    sx, sy,
                    0
                );
            }
        }
        if (door && door->Visible) {
            auto bmp = door->Bitmap.get();
            if (bmp) {
                float px = mapX + (door->Position.x - door->Size.x / 2) * scale;
                float py = mapY + (door->Position.y - door->Size.y / 2) * scale;
                float sx = door->Size.x * scale;
                float sy = door->Size.y * scale;

                al_draw_scaled_bitmap(
                    bmp,
                    0, 0,
                    al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
                    px, py,
                    sx, sy,
                    0
                );
            }
        }

    }

    // Reset clipping to full screen
    al_set_clipping_rectangle(0, 0, screenWidth, screenHeight);

    // Draw border
    al_draw_rounded_rectangle(mapX, mapY, mapX + scaledWidth, mapY + scaledHeight, 5, 5, al_map_rgb(255, 255, 255), 5.0f);

    // Position the button at the top-left corner of the map
    Map_btn->Position.x = mapX + border;
    Map_btn->Position.y = mapY + border;
    Map_btn->Size.x = scaledWidth;
    Map_btn->Size.y = scaledHeight;
}

void PlayScene::FlashLight() const {
    if (!player1 || !player2) return;

    // Set the target to our mask bitmap
    al_set_target_bitmap(mask);
    const float radius = BlockSize * 2.f;

    // Clear to completely black (transparent)
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    // Draw white circles where we want light (opaque)
    for (float i = radius; i > 0; i-=2.f) {
        al_draw_filled_circle(player1->Position.x, player1->Position.y + abs(player1->Size.y/2),
                             i, al_map_rgba(255, 255, 255, 255/i * 2.f));
        al_draw_filled_circle(player2->Position.x, player2->Position.y + abs(player1->Size.y/2),
                             i, al_map_rgba(255, 255, 255, 255/i * 2.f));
    }

    // Switch back to main display
    al_set_target_bitmap(al_get_backbuffer(al_get_current_display()));

    // Use the mask to determine what's visible
    al_set_blender(ALLEGRO_ADD, ALLEGRO_DEST_COLOR, ALLEGRO_ZERO);
    al_draw_bitmap(mask, 0, 0, 0);

    // Set normal blending for other drawings
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
}

void PlayScene::MiniMapOnClick(int stage) {
    isMap_clicked = !isMap_clicked;
}

void PlayScene::ReadEnemyWave() {
    //spawn point y, type, count, cooldown, left/right (1/0)
    enemyWave.clear();
    std::string filename = "Resource/enemy" + std::to_string(MapId) + ".txt";
    std::ifstream ifs(filename, std::ios_base::in);

    if (ifs.is_open()) {
        std::string str;
        while (getline(ifs, str)){
            if (str[0] == '#' || str.empty()) {
                str.clear();
                continue;
            }

            std::stringstream ss(str);
            float pos_y, typ, cnt, cd, dir;
            while (
            ss >> pos_y &&
            ss >> typ &&
            ss >> cnt &&
            ss >> cd &&
            ss >> dir
            ) {
                    enemyWave.push_back( {
                    pos_y,
                    typ,
                    cnt,
                    cd,
                    dir
                });

                pos_y = 0;
                typ = 0;
                cnt = 0;
                cd = 0;
                dir = 0;
            }

            str.clear();
        }

        ifs.close();
    } else {
        std::cerr << "[ERROR] Could not open ENEMY WAVE TXT for reading!" << std::endl;
    }
}

void PlayScene::ConstructUI() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2, halfH = h / 2;

}

void PlayScene::UIBtnClicked(int id) {


}

void PlayScene::HomeOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}


//-----------For Pause UI-------------------------

void PlayScene::CreatePauseUI() {
    //get width and height
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x + Camera.x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y + Camera.y;
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

    //to continue perhaps
    continueButton = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", btnPosX, btnPosY + 100, 400, 100);
    continueButton->Visible = false;
    continueButton->Enabled = false;
    UIGroup->AddNewControlObject(continueButton);
    continueButton->SetOnClickCallback(std::bind(&PlayScene::ContinueOnClick, this, 1));

    //restart if frustrated
    restartButton = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", btnPosX, btnPosY + 230, 400, 100);
    restartButton->Visible = false;
    restartButton->Enabled = false;
    UIGroup->AddNewControlObject(restartButton);
    restartButton->SetOnClickCallback(std::bind(&PlayScene::RestartOnClick, this, 1));

    //exit to stage scene
    exitButton = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", btnPosX, btnPosY + 360, 400, 100);
    exitButton->Visible = false;
    exitButton->Enabled = false;
    UIGroup->AddNewControlObject(exitButton);
    exitButton->SetOnClickCallback(std::bind(&PlayScene::BackOnClick, this, 1));

    //the label
    continueLabel = new Engine::Label("Continue", "pirulen.ttf", 48, lblPosX, lblPosY + 150, 0, 0, 0, 255, 0.5, 0.5);
    continueLabel->Visible = false;
    UIGroup->AddNewObject(continueLabel);

    restartLabel = new Engine::Label("Retry", "pirulen.ttf", 48, lblPosX, lblPosY + 280, 0, 0, 0, 255, 0.5, 0.5);
    restartLabel->Visible = false;
    UIGroup->AddNewObject(restartLabel);

    exitLabel = new Engine::Label("Exit", "pirulen.ttf", 48, lblPosX, lblPosY + 410, 0, 0, 0, 255, 0.5, 0.5);
    exitLabel->Visible = false;
    UIGroup->AddNewObject(exitLabel);

    //slider
    sliderBGM = new Slider(40 + halfW - 95, halfH - 200 - 2, 190, 4);
    sliderBGM->Visible = false;
    sliderBGM->Enabled = false;
    sliderBGM->SetValue(AudioHelper::BGMVolume);
    sliderBGM->SetOnValueChangedCallback(std::bind(&PlayScene::BGMSlideOnValueChanged, this, std::placeholders::_1));
    UIGroup->AddNewControlObject(sliderBGM);

    BGMSlider = new Engine::Label("BGM: ", "pirulen.ttf", 28, 40 + halfW - 60 - 95, halfH - 200, 0, 0, 0, 255, 0.5, 0.5);
    BGMSlider->Visible = false;
    UIGroup->AddNewObject(BGMSlider);

    //slider
    sliderSFX = new Slider(40 + halfW - 95, halfH - 150 + 2, 190, 4);
    sliderSFX->Visible = false;
    sliderSFX->Enabled = false;
    sliderSFX->SetValue(AudioHelper::SFXVolume);
    sliderSFX->SetOnValueChangedCallback(std::bind(&PlayScene::SFXSlideOnValueChanged, this, std::placeholders::_1));
    UIGroup->AddNewControlObject(sliderSFX);

    SFXSlider = new Engine::Label("SFX: ", "pirulen.ttf", 28, 40 + halfW - 60 - 95, halfH - 150, 0, 0, 0, 255, 0.5, 0.5);
    SFXSlider->Visible = false;
    UIGroup->AddNewObject(SFXSlider);

    //enable 2nd player
    enable2nd = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", lblPosX - 200,  lblPosY - 350, 400, 100);
    UIGroup->AddNewControlObject(enable2nd);
    enable2nd->SetOnClickCallback(std::bind(&PlayScene::Enable2ndPlayer, this, 1));
    enable2nd->Visible = false;
    enable2nd->Enabled = false;

    enable2ndLabel = new Engine::Label("2nd Enabled", "pirulen.ttf", 36, lblPosX - 200 + 24, lblPosY - 336, 0, 0, 0, 255, 0, 0);
    enable2ndLabel->Visible = false;
    UIGroup->AddNewObject(enable2ndLabel);
}

//-------For Exit, restart, and Continue Button------------------
void PlayScene::BackOnClick(int state) {

    if (SelectProfileScene::isSaved) {
        std::time_t now = std::time(nullptr);

        // Convert to local time
        std::tm tm{};

        #ifdef _WIN32
            localtime_s(&tm, &now);  // Windows
        #else
            localtime_r(&now, &tm);  // Linux / macOS
        #endif

        // Format date with slashes: YYYY/MM/DD
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y/%m/%d  %H:%M");
        std::string date_time = oss.str();

        SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Last_Played = date_time;
        SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Duration = std::to_string(std::stof(SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Duration) + total_time);
        SelectProfileScene::WriteProfileData(nullptr);
    }

    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void PlayScene::ContinueOnClick(int state) {
    IsPaused = false;
}

void PlayScene::RestartOnClick(int state) {
    IsPaused = false;

    if (SelectProfileScene::isSaved) {
        std::time_t now = std::time(nullptr);

        // Convert to local time
        std::tm tm{};

        #ifdef _WIN32
            localtime_s(&tm, &now);  // Windows
        #else
            localtime_r(&now, &tm);  // Linux / macOS
        #endif

        // Format date with slashes: YYYY/MM/DD
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y/%m/%d  %H:%M");
        std::string date_time = oss.str();

        SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Last_Played = date_time;
        SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Duration = std::to_string(std::stof(SelectProfileScene::playerData[SelectProfileScene::getProfileID()-1].Duration) + total_time);
        SelectProfileScene::WriteProfileData(nullptr);
    }

    Engine::GameEngine::GetInstance().ChangeScene("play");
}
//volume
void PlayScene::BGMSlideOnValueChanged(float value) {
    AudioHelper::BGMVolume = value;
    AudioHelper::ChangeSampleVolume(bgmInstance, value);
}
void PlayScene::SFXSlideOnValueChanged(float value) {
    AudioHelper::SFXVolume = value;
}

//---For Pause---------------->>>>>>>>>>>>>>>>>>
void PlayScene::UpdatePauseState() {
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
    if (restartButton) {
        restartButton->Visible = show;
        restartButton->Enabled = show;
        restartLabel->Visible = show;
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
    if (enable2nd) {
        enable2nd->Visible = show;
        enable2nd->Enabled = show;
        enable2ndLabel->Visible = show;
    }
}

void PlayScene::Enable2ndPlayer(int stage) {
    if (!player1) return;
    enable2ndplayer = !enable2ndplayer;
    if (enable2ndplayer) {
        player1->Visible = true;
        enable2ndLabel->Text = "2nd Enabled";
    }
    else {
        player1->Visible = false;
        enable2ndLabel->Text = "2nd Disabled";
    }
}

void PlayScene::MazeCreator() {
    std::string filename = "Resource/map5.txt";
    const int width = 25;
    const int height = 25;
    std::vector<std::string> map(height, std::string(width, '9'));

    // Random engine
    std::mt19937 rng(static_cast<unsigned>(time(0)));

    // Maze grid generation (Perfect Maze using DFS)
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    std::function<void(int, int)> dfs = [&](int x, int y) {
        visited[y][x] = true;
        map[y][x] = '8'; // Mark path

        std::vector<std::pair<int, int>> directions = {{0, -2}, {-2, 0}, {0, 2}, {2, 0}};
        std::shuffle(directions.begin(), directions.end(), rng);

        for (auto [dx, dy] : directions) {
            int nx = x + dx, ny = y + dy;
            if (nx > 0 && ny > 0 && nx < width - 1 && ny < height - 1 && !visited[ny][nx]) {
                map[y + dy / 2][x + dx / 2] = '8'; // Remove wall between
                dfs(nx, ny);
            }
        }
    };

    dfs(1, 1); // Start maze generation from (1, 1)

    // Set wall borders
    for (int i = 0; i < width; ++i) {
        map[0][i] = '9';
        map[height - 1][i] = '9';
    }
    for (int i = 0; i < height; ++i) {
        map[i][0] = '9';
        map[i][width - 1] = '9';
    }

    // Place players at top-left
    map[1][1] = 'N';
    map[1][2] = 'T';

    // Collect all open path positions
    std::vector<std::pair<int, int>> openTiles;
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            if (map[y][x] == '8')
                openTiles.emplace_back(y, x);
        }
    }

    std::shuffle(openTiles.begin(), openTiles.end(), rng);

    if (openTiles.size() < 3) {
        throw std::runtime_error("Not enough open tiles to place buttons and portal.");
    }

    // Place two distinct pressure plates
    auto [b1y, b1x] = openTiles[0];
    auto [b2y, b2x] = openTiles[1];
    map[b1y][b1x] = '5';
    map[b2y][b2x] = '5';

    // Place portal as far from start as possible (bottom-right bias)
    bool portalPlaced = false;
    for (const auto& [y, x] : openTiles) {
        if (map[y][x] == '8' && y > height / 2 && x > width / 2) {
            map[y][x] = '6';
            portalPlaced = true;
            break;
        }
    }

    if (!portalPlaced) {
        for (const auto& [y, x] : openTiles) {
            if (map[y][x] == '8') {
                map[y][x] = '6';
                break;
            }
        }
    }

    // Write to file
    std::ofstream outFile(filename);
    outFile << width << " " << height << "\n";
    for (const auto& row : map)
        outFile << row << "\n";
    outFile.close();
}




//DIALOGUE FUNCTION====================================================================================================
void PlayScene::RenderDialog() const {
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
            const float imgWidth = BlockSize * 1.5f;
            const float imgHeight = BlockSize * 1.8f;
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

void PlayScene::StartDialog(const std::vector<Dialog>& dialogs, bool skippable = true) {
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

void PlayScene::UpdateDialog(float deltaTime) {
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


//for player removal
void PlayScene::RemovePlayer(Player* player) {
    if (!player) return;

    PlayerGroup->RemoveObject(player->GetObjectIterator());
    if (player == player1) player1 = nullptr;
    if (player == player2) player2 = nullptr;
    delete player;
}

void PlayScene::UpdateHealthBarPositions() {
    float padding = BlockSize *0.1;
    float headSize = BlockSize*0.8;
    float healthBarWidth = BlockSize * 3;
    float healthBarHeight = BlockSize/3;

    float yPos = padding;

    // Update Player 1 health bar
    if (player1) {
        healthBarP1.head->Position = Engine::Point(Camera.x+padding,Camera.y + yPos);
        healthBarP1.bg->Position = Engine::Point(Camera.x + headSize,Camera.y + yPos + BlockSize/2.5);
        healthBarP1.fg->Position = healthBarP1.bg->Position;
        healthBarP1.fg->Size.x = healthBarWidth * (player1->hp / player1->MaxHp);
        std::cout << (player1->hp / player1->MaxHp) << std::endl;
        healthBarP1.bg->Size.x = healthBarWidth;

        healthBarP1.head->Visible = player1->Visible;
        healthBarP1.bg->Visible = player1->Visible;
        healthBarP1.fg->Visible = player1->Visible;

        yPos += headSize + 2*padding;
    }

    // Update Player 2 health bar
    if (player2) {
        healthBarP2.head->Position = Engine::Point( Camera.x + padding,Camera.y + yPos);
        healthBarP2.bg->Position = Engine::Point(Camera.x + headSize,Camera.y + yPos + BlockSize/2.5);
        healthBarP2.fg->Position = healthBarP2.bg->Position;
        healthBarP2.fg->Size.x = healthBarWidth * (player2->hp / player2->MaxHp);

        healthBarP2.head->Visible = player2->Visible;
        healthBarP2.bg->Visible = player2->Visible;
        healthBarP2.fg->Visible = player2->Visible;
    }
}