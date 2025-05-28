#include <algorithm>
#include <allegro5/allegro.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "LeaderboardScene.hpp"

#include <allegro5/allegro_primitives.h>

#include "Player/Player.h"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"

namespace Engine {
    class ImageButton;
}

int PlayScene::MapWidth = 0, PlayScene::MapHeight = 0;
bool pressed;
bool PlayScene::DebugMode = false;
const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::BlockSize = 64;
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
    mapState.clear();
    keyStrokes.clear();
    ticks = 0;
    deathCountDown = -1;
    lives = 10;
    money = 150;
    SpeedMult = 1;
    // Add groups from bottom to top.
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(DebugIndicatorGroup = new Group());
    AddNewObject(EffectGroup = new Group());
    AddNewObject(PlayerGroup = new Group());
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    ReadMap();
    ReadEnemyWave();
    ConstructUI();
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    preview = nullptr;
    UIGroup->AddNewObject(imgTarget);
    // Preload Lose Scene
    deathBGMInstance = Engine::Resources::GetInstance().GetSampleInstance("astronomia.ogg");
    Engine::Resources::GetInstance().GetBitmap("lose/benjamin-happy.png");
    // Start BGM.
    bgmId = AudioHelper::PlayBGM("play.ogg");
}
void PlayScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
    PlayerGroup->Update(deltaTime);
}
void PlayScene::Draw() const {
    IScene::Draw();
    PlayerGroup->Draw();
}
void PlayScene::OnMouseDown(int button, int mx, int my) {

}
void PlayScene::OnMouseMove(int mx, int my) {

}
void PlayScene::OnMouseUp(int button, int mx, int my) {

}
void PlayScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_TAB) {
        DebugMode = !DebugMode;
    }
    if (keyCode >= ALLEGRO_KEY_0 && keyCode <= ALLEGRO_KEY_9) {
        // Hotkey for Speed up.
        SpeedMult = keyCode - ALLEGRO_KEY_0;
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
            case 'P': mapData.push_back('P'); break;
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
                case 'P':
                    mapState[i][j]=TILE_AIR;
                    break;
                default:
                    mapState[i][j]=TILE_AIR;
                    break;

            }
            if (num=='0')
                TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            else if (num=='1') {
                TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth] != '1')
                    TileMapGroup->AddNewObject(new Engine::Image("play/grass-1.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if((mapData[idx-1])!='1'&&idx%MapWidth!=0)
                    TileMapGroup->AddNewObject(new Engine::Image("play/grass-2.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx+1] != '1'&&idx%MapWidth!=(MapWidth-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/grass-3.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if((mapData[idx-MapWidth-1])!='1'&&idx%MapWidth!=0)
                    TileMapGroup->AddNewObject(new Engine::Image("play/grass-4.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if (mapData[idx-MapWidth+1] != '1'&&idx%MapWidth!=(MapWidth-1))
                    TileMapGroup->AddNewObject(new Engine::Image("play/grass-5.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }
            else if (num=='2') {
                TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                if ((mapData[idx-1]=='1'&&mapData[idx+1]=='1')&&idx%MapWidth!=(MapWidth-1)&&idx%MapWidth!=0)
                    TileMapGroup->AddNewObject(new Engine::Image("play/platform-4.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                else if (mapData[idx-1]=='1')
                    TileMapGroup->AddNewObject(new Engine::Image("play/platform-2.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                else if (mapData[idx+1]=='1')
                    TileMapGroup->AddNewObject(new Engine::Image("play/platform-3.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                else if (mapData[idx-1]=='2'||mapData[idx+1]=='2')
                    TileMapGroup->AddNewObject(new Engine::Image("play/platform-1.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            }
            else if (num=='P') {
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize / 2, i * BlockSize + BlockSize / 2);
                TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                PlayerGroup->AddNewObject(new Player("play/1panda.png",SpawnCoordinate.x,SpawnCoordinate.y,BlockSize,192,100));
            }
        }
    }
    // for (int i = 0; i < MapHeight; i++) {
    //     for (int j = 0; j < MapWidth; j++) {
    //         if (num)
    //             TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
    //         else
    //             TileMapGroup->AddNewObject(new Engine::Image("play/dirt.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
    //     }
    // }
}
void PlayScene::ReadEnemyWave() {

}
void PlayScene::ConstructUI() {

}

void PlayScene::UIBtnClicked(int id) {


}

void PlayScene::HomeOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void PlayScene::RestartOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("play");
}
