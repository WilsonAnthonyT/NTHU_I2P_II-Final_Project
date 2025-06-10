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
#include "InteractiveBlock/Box.h"
#include "InteractiveBlock/Sensor.h"
#include "Player/MazePlayerA.h"
#include "Player/MazePlayerB.h"
#include "Player/TankPlayerA.h"
#include "Player/TankPlayerB.h"
#include "UI/Animation/DamageText.h"

namespace Engine {
    class ImageButton;
}

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


    Camera.x=0,Camera.y=0;
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 10;
    money = 150;
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
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    ReadMap();


    if (MapId == 3) {
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

    ReadEnemyWave();
    ConstructUI();

    if (MapId == 1 || MapId == 2 || MapId == 3 || MapId == 4 || MapId == 5) {
        backgroundIMG = Engine::Resources::GetInstance().GetBitmap("play/background1.png");
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


    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    preview = nullptr;
    UIGroup->AddNewObject(imgTarget);
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    bgmInstance = AudioHelper::PlaySample("play.ogg", true, AudioHelper::BGMVolume);
    CreatePauseUI();

    //dialogue
    // dialogFont = al_load_font("Resource/fonts/pirulen.ttf", 36, 0);
    // if (!dialogFont) {
    //     throw std::runtime_error("Failed to load font: Resource/fonts/pirulen.ttf");
    // }
    // std::vector<Dialog> dialogs = {
    //     {"Quick! The enemy is attacking!", 3.0f},
    //     {"Prepare your defenses!", 3.0f},
    //     {"Good luck, soldier!", 3.0f}
    // };
    // StartDialog(dialogs);
}
void PlayScene::Terminate() {
    if (dialogFont) {
        al_destroy_font(dialogFont);
        dialogFont = nullptr;
    }
    AudioHelper::StopSample(bgmInstance);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
    // if (enemyWaveData.empty() && EnemyGroup->GetObjects().empty()) {
    //     MapId++;
    //     Engine::GameEngine::GetInstance().ChangeScene("play");
    // }

    UpdatePauseState();
    if (IsPaused) {
        UIGroup->Update(deltaTime);
        return;
    }
    if (currentState == GameState::Dialog) {
        UpdateDialog(deltaTime);
        UIGroup->Update(deltaTime);
        return; // Skip other updates while in dialog
    }

    for (auto& p : rainParticles) {
        p.y += p.speed * deltaTime * BlockSize; // Multiply by deltaTime for smooth speed
        p.x += 0.5f * deltaTime;    // Optional: slight wind effect

        // Reset particles that go off-screen
        if (p.y > PlayScene::MapHeight * PlayScene::BlockSize) {
            p.y = -10.0f; // Start above the screen
            p.x = rand() % (PlayScene::MapWidth * PlayScene::BlockSize);
        }
    }

    BulletGroup->Update(deltaTime);
    EnemyBulletGroup->Update(deltaTime);
    WeaponGroup->Update(deltaTime);
    PlayerGroup->Update(deltaTime);
    EnemyGroup->Update(deltaTime);
    DamageTextGroup->Update(deltaTime);
    EffectGroup->Update(deltaTime);
    InteractiveBlockGroup->Update(deltaTime);

    //players
    std::vector<Player*> players;
    for (auto& it : PlayerGroup->GetObjects()) {
        Player *player = dynamic_cast<Player *>(it);
        if (player) {
            players.push_back(player);
        }
    }

    for (auto& it : players) {
        if (it->hp <= 0) {
            PlayerGroup->RemoveObject(it->GetObjectIterator());
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

    if (player1->Visible == true && player2->Visible == true) {
        Engine::Point target = (players[0]->Position + players[1]->Position)/2;
        Camera.x += (target.x - screenWidth / 2 - Camera.x) * 0.1f;
        Camera.y += (target.y - screenHeight / 2 - Camera.y) * 0.1f;
        // Camera.x = (target.x - screenWidth / 2);
        // Camera.y = (target.y - screenHeight / 2);
        if (Camera.x < 0)Camera.x = 0;
        if (Camera.x > MapWidth * BlockSize - screenWidth)Camera.x = MapWidth * BlockSize - screenWidth;
        if (Camera.y < 0)Camera.y = 0;
        if (Camera.y > MapHeight * BlockSize - screenHeight)Camera.y = MapHeight * BlockSize - screenHeight;
    }
    else {
        Engine::Point target = players[1]->Position;
        Camera.x = (target.x - screenWidth / 2);
        Camera.y = (target.y - screenHeight / 2);
        if (Camera.x < 0)Camera.x = 0;
        if (Camera.x > MapWidth * BlockSize - screenWidth)Camera.x = MapWidth * BlockSize - screenWidth;
        if (Camera.y < 0)Camera.y = 0;
        if (Camera.y > MapHeight * BlockSize - screenHeight)Camera.y = MapHeight * BlockSize - screenHeight;
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

    if (MapId == 3) FlashLight();

    PlayerGroup->Draw();
    WeaponGroup->Draw();
    EffectGroup->Draw();
    EnemyBulletGroup->Draw();

    al_identity_transform(&trans);
    al_use_transform(&trans);

    //for map debug
    MiniMap();
    if (currentState == GameState::Dialog) {
        RenderDialog();
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
}
int PlayScene::GetMoney() const {
    return money;
}
void PlayScene::EarnMoney(int money) {
    this->money += money;
    UIMoney->Text = std::string("$") + std::to_string(this->money);
}
void PlayScene::ReadMap() {
    if (MapId==3)MazeCreator();
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
            case 'T': mapData.push_back('T'); break;
            case '3': mapData.push_back('3'); break;
            case '4': mapData.push_back('4'); break;
            case '5': mapData.push_back('5'); break;
            case '6': mapData.push_back('6'); break;
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
                // if((mapData[idx-1])!='1'&& idx%MapWidth!=0&&(mapData[idx-MapWidth])=='1')
                //     TileMapGroup->AddNewObject(new Engine::Image("play/grass-2.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // if (mapData[idx+1] != '1'&& idx%MapWidth!=(MapWidth-1)&&(mapData[idx-MapWidth])=='1')
                //     TileMapGroup->AddNewObject(new Engine::Image("play/grass-3.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // if((mapData[idx-1])!='1'&& idx%MapWidth!=0&&(mapData[idx-MapWidth])!='1')
                //     TileMapGroup->AddNewObject(new Engine::Image("play/grass-6.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // if (mapData[idx+1] != '1'&& idx%MapWidth!=(MapWidth-1)&&(mapData[idx-MapWidth])!='1')
                //     TileMapGroup->AddNewObject(new Engine::Image("play/grass-7.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // if((mapData[idx-MapWidth-1])!='1'&& idx%MapWidth!=0 && mapData[idx-MapWidth] == '1' && mapData[idx-1] == '1')
                //     TileMapGroup->AddNewObject(new Engine::Image("play/grass-4.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                // if (mapData[idx-MapWidth+1] != '1'&& idx%MapWidth!=(MapWidth-1) && mapData[idx-MapWidth] == '1' && mapData[idx+1] == '1')
                //     TileMapGroup->AddNewObject(new Engine::Image("play/grass-5.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
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
                EnemyGroup->AddNewObject(new EjojoEnemy(EnemySpawnCoordinate.x, EnemySpawnCoordinate.y));
            }else if (num == 'F') {
                Engine::Point EnemySpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                EnemyGroup->AddNewObject(new MiniEjojo(EnemySpawnCoordinate.x, EnemySpawnCoordinate.y));
            }else if (num == 'S') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                InteractiveBlockGroup->AddNewObject(new Sensor("play/sensor.png",SpawnCoordinate.x, SpawnCoordinate.y,2));
            }else if (num == 'N') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                player1 = (new MazePlayerA(SpawnCoordinate.x, SpawnCoordinate.y));
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                PlayerGroup->AddNewObject(player1);
            }
            else if (num == 'T') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                player2 = (new MazePlayerB(SpawnCoordinate.x, SpawnCoordinate.y));
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                PlayerGroup->AddNewObject(player2);
            } else if (num=='3'){
                TileMapGroup->AddNewObject(new Engine::Image("play/tool-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            } else if (num=='4') {
                TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }else if (num=='5'){
                TileMapGroup->AddNewObject(new Engine::Image("play/tower-base.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            } else if (num=='6') {
                TileMapGroup->AddNewObject(new Engine::Image("play/explosion-3.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }
        }
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
    const ALLEGRO_COLOR p1_color = al_map_rgb(255, 0, 0);
    const ALLEGRO_COLOR p2_color = al_map_rgb(0, 255, 0);
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
            if (mapState[y][x] != TILE_AIR) {
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

            al_draw_filled_circle(px, py, 3.0f, (player == player1) ? p1_color : p2_color);
        }
    }

    // Reset clipping to full screen
    al_set_clipping_rectangle(0, 0, screenWidth, screenHeight);

    // Border on top (over the clipped content)
    al_draw_rounded_rectangle(miniX, miniY, miniX + miniWidth, miniY + miniHeight, 5, 5, al_map_rgb(255, 255, 255), 3.0f);
}


// void PlayScene::FlashLight() const {
//     //al_clear_to_color(al_map_rgb(0, 0, 0));
//     al_draw_filled_circle(player1->Position.x, player1->Position.y + abs(player1->Size.y/2), BlockSize, al_map_rgb(255, 255, 255));
//     al_draw_filled_circle(player2->Position.x, player2->Position.y + abs(player1->Size.y/2), BlockSize, al_map_rgb(255, 255, 255));
//
//     al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
// }

void PlayScene::FlashLight() const {
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

void PlayScene::ReadEnemyWave() {

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
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void PlayScene::ContinueOnClick(int state) {
    IsPaused = false;
}

void PlayScene::RestartOnClick(int state) {
    IsPaused = false;
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
    std::string filename = "Resource/map3.txt";
    const int width = 25;
    const int height = 25;
    std::vector<std::string> map(height, std::string(width, '4'));

    // Random engine
    std::mt19937 rng(static_cast<unsigned>(time(0)));

    // Maze grid generation (Perfect Maze using DFS)
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    std::function<void(int, int)> dfs = [&](int x, int y) {
        visited[y][x] = true;
        map[y][x] = '3'; // Mark path

        std::vector<std::pair<int, int>> directions = {{0, -2}, {-2, 0}, {0, 2}, {2, 0}};
        std::shuffle(directions.begin(), directions.end(), rng);

        for (auto [dx, dy] : directions) {
            int nx = x + dx, ny = y + dy;
            if (nx > 0 && ny > 0 && nx < width - 1 && ny < height - 1 && !visited[ny][nx]) {
                map[y + dy / 2][x + dx / 2] = '3'; // Remove wall between
                dfs(nx, ny);
            }
        }
    };

    dfs(1, 1); // Start maze generation from (1, 1)

    // Set wall borders
    for (int i = 0; i < width; ++i) {
        map[0][i] = '4';
        map[height - 1][i] = '4';
    }
    for (int i = 0; i < height; ++i) {
        map[i][0] = '4';
        map[i][width - 1] = '4';
    }

    // Place players at top-left
    map[1][1] = 'N';
    map[1][2] = 'T';

    // Collect all open path positions
    std::vector<std::pair<int, int>> openTiles;
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            if (map[y][x] == '3')
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
        if (map[y][x] == '3' && y > height / 2 && x > width / 2) {
            map[y][x] = '6';
            portalPlaced = true;
            break;
        }
    }

    if (!portalPlaced) {
        for (const auto& [y, x] : openTiles) {
            if (map[y][x] == '3') {
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


void PlayScene::RenderDialog() const {
    const int screenW = Engine::GameEngine::GetInstance().GetScreenWidth();
    const int screenH = Engine::GameEngine::GetInstance().GetScreenHeight();

    // Draw a semi-transparent box
    al_draw_filled_rectangle(screenW * 0.1, screenH * 0.8, screenW * 0.9, screenH * 0.95, al_map_rgba(0, 0, 0, 200));
    al_draw_rectangle(screenW * 0.1, screenH * 0.8, screenW * 0.9, screenH * 0.95, al_map_rgb(255, 255, 255), 2);

    // Draw dialog text
    ALLEGRO_FONT* dialogFont = al_load_font("Resource/fonts/pirulen.ttf", 24, 0); // Load font with size 24
    if (!dialogFont) {
        throw std::runtime_error("Failed to load font: pirulen.ttf");
    }
    if (dialogFont) {
        al_draw_text(
            dialogFont,
            al_map_rgb(255, 255, 255),
            screenW * 0.5,
            screenH * 0.825,
            ALLEGRO_ALIGN_CENTER,
            currentDialogText.c_str()
        );
    }
}

void PlayScene::StartDialog(const std::vector<Dialog> &dialogs) {
    while (!dialogQueue.empty()) dialogQueue.pop(); // Clear old dialogs
    for (const auto& d : dialogs) {
        dialogQueue.push(d);
    }
    currentState = GameState::Dialog;
    if (!dialogQueue.empty()) {
        currentDialogText = dialogQueue.front().text;
        dialogTimer = dialogQueue.front().duration;
        dialogQueue.pop();
    }
}

void PlayScene::UpdateDialog(float deltaTime) {
    if (currentState == GameState::Dialog) {
        dialogTimer -= deltaTime;
        if (dialogTimer <= 0) {
            if (!dialogQueue.empty()) {
                currentDialogText = dialogQueue.front().text;
                dialogTimer = dialogQueue.front().duration;
                dialogQueue.pop();
            } else {
                currentState = GameState::Normal; // Exit dialog state
            }
        }
    }
}
