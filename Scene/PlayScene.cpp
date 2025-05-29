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

#include <map>

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
int PlayScene::BlockSize = 0;
float PlayScene::Gravity = 0;
const float PlayScene::DangerTime = 7.61;
Engine::Point PlayScene::SpawnGridPoint = Engine::Point(-1, 0);
const std::vector<int> PlayScene::code = {
    ALLEGRO_KEY_UP, ALLEGRO_KEY_UP, ALLEGRO_KEY_DOWN, ALLEGRO_KEY_DOWN,
    ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT, ALLEGRO_KEY_LEFT, ALLEGRO_KEY_RIGHT,
    ALLEGRO_KEY_B, ALLEGRO_KEY_A, ALLEGRO_KEY_LSHIFT, ALLEGRO_KEY_ENTER
};
Player *player1;
Engine::Point PlayScene::GetClientSize() {
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);

}
void PlayScene::Initialize() {
    screenWidth = Engine::GameEngine::GetInstance().GetScreenWidth();
    screenHeight = Engine::GameEngine::GetInstance().GetScreenHeight();
    BlockSize = screenWidth / 16;
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
    bgmInstance = AudioHelper::PlaySample("play.ogg", true, AudioHelper::BGMVolume);
    CreatePauseUI();
}
void PlayScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
    UpdatePauseState();
    if (IsPaused) {
        UIGroup->Update(deltaTime);
        return;
    }
    PlayerGroup->Update(deltaTime);

    Engine::Point target = player1->Position;
    Camera.x += (target.x - screenWidth / 2 - Camera.x) * 0.1f;
    Camera.y += (target.y - screenHeight / 2 - Camera.y) * 0.1f;
    // Camera.x = (target.x - screenWidth / 2);
    // Camera.y = (target.y - screenHeight / 2);
    if (Camera.x < 0)Camera.x = 0;
    if (Camera.x > MapWidth * BlockSize - screenWidth)Camera.x = MapWidth * BlockSize - screenWidth;
    if (Camera.y < 0)Camera.y = 0;
    if (Camera.y > MapHeight * BlockSize - screenHeight)Camera.y = MapHeight * BlockSize - screenHeight;
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
    IScene::Draw();            // will draw tiles/UI, now offset by camera
    PlayerGroup->Draw();       // players, effects, etc.
    al_identity_transform(&trans);
    al_use_transform(&trans);
    UIGroup->Draw();

    //for map debug
    if (DebugMode) {
        for (int i = 0; i < MapHeight; i++) {
            for (int j = 0; j < MapWidth; j++) {
                if (mapState[i][j] == TILE_DIRT || mapState[i][j] == TILE_WPLATFORM) {
                    float x = j * BlockSize - Camera.x;
                    float y = i * BlockSize - Camera.y;
                    ALLEGRO_COLOR color = (mapState[i][j] == TILE_DIRT) ? al_map_rgb(255, 0, 0) : al_map_rgb(0, 255, 0);
                    al_draw_rectangle(x, y, x + BlockSize, y + BlockSize, color, 2.0);
                }
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

    if (keyCode == ALLEGRO_KEY_TAB) {
        DebugMode = !DebugMode;
    }
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
                Engine::Point SpawnCoordinate = Engine::Point( j * BlockSize + BlockSize/2, i * BlockSize);
                TileMapGroup->AddNewObject(new Engine::Image("play/floor.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
                PlayerGroup->AddNewObject(player1 = new Player("play/1panda.png",SpawnCoordinate.x,SpawnCoordinate.y,BlockSize,BlockSize * 2.25,100));
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
}

//-------For Exit, restart, and Continue Button------------------
void PlayScene::BackOnClick(int state) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
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
}
