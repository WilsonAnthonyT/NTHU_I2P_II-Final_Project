#include "CreditScene.h"
#include "Engine/GameEngine.hpp"
#include <allegro5/allegro_primitives.h>

#include "Engine/Resources.hpp"
#include "UI/Component/Label.hpp"

CreditScene::CreditScene() :
    scrollY(0),
    isScrolling(true),
    creditGroup(nullptr),
    UIGroup(nullptr),
    backButton(nullptr) {}

CreditScene::~CreditScene() {
    Terminate();
}

void CreditScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    scrollSpeed=w/(16*4);
    if (AudioHelper::sharedBGMInstance) {
        AudioHelper::StopSample(AudioHelper::sharedBGMInstance);
        AudioHelper::sharedBGMInstance = nullptr;
    }
    bgmInstance = AudioHelper::PlaySample("victory.mp3", true, AudioHelper::BGMVolume);
    // Create groups
    AddNewObject(creditGroup = new Engine::Group());
    AddNewControlObject(UIGroup = new Engine::Group());

    // Add credits directly
    float currentY = PlayScene::BlockSize;

    // Title
    creditGroup->AddNewObject(new Engine::Label(
        "Invasion Requiem", "pirulen.ttf", PlayScene::BlockSize,
        halfW, currentY, 10, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize * 1.5f;

    // Version
    creditGroup->AddNewObject(new Engine::Label(
        "Version 1.0", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize * 1.5f;

    // Spacer
    currentY += PlayScene::BlockSize;

    // Development Team
    creditGroup->AddNewObject(new Engine::Label(
        "Lost-in-taiwan.co.id", "pirulen.ttf", PlayScene::BlockSize/2,
        halfW, currentY, 100, 200, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize * 1.5f;

    creditGroup->AddNewObject(new Engine::Label(
        "Lead Developer", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 215, 0, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;
    creditGroup->AddNewObject(new Engine::Label(
        "Feivel \"anotherfei\" Kusuma", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;

    creditGroup->AddNewObject(new Engine::Label(
            "Wilson \"wilsonanthonyt\" Tang", "pirulen.ttf", PlayScene::BlockSize/3,
            halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;
    creditGroup->AddNewObject(new Engine::Label(
            "michael \"mihu21\" huang", "pirulen.ttf", PlayScene::BlockSize/3,
            halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize*1.5;

    creditGroup->AddNewObject(new Engine::Label(
        "Game Developer", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 215, 0, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;
    creditGroup->AddNewObject(new Engine::Label(
        "Feivel \"anotherfei\" Kusuma", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;

    creditGroup->AddNewObject(new Engine::Label(
            "Wilson \"wilsonanthonyt\" Tang", "pirulen.ttf", PlayScene::BlockSize/3,
            halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    creditGroup->AddNewObject(new Engine::Image(
        "start/bryan.png", w-PlayScene::BlockSize*2.5, currentY,
        PlayScene::BlockSize*3, PlayScene::BlockSize*4, 0.5, 0.5));
    currentY += PlayScene::BlockSize;
    creditGroup->AddNewObject(new Engine::Label(
            "michael \"mihu21\" huang", "pirulen.ttf", PlayScene::BlockSize/3,
            halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    creditGroup->AddNewObject(new Engine::Image(
        "start/arwen.png", PlayScene::BlockSize*2.5, currentY,
        PlayScene::BlockSize*3, PlayScene::BlockSize*4, 0.5, 0.5));
    currentY += PlayScene::BlockSize*1.5;

    creditGroup->AddNewObject(new Engine::Label(
        "Map Developer", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 215, 0, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;

    creditGroup->AddNewObject(new Engine::Label(
        "Feivel \"anotherfei\" Kusuma", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;

    creditGroup->AddNewObject(new Engine::Label(
            "Wilson \"wilsonanthonyt\" Tang", "pirulen.ttf", PlayScene::BlockSize/3,
            halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;
    creditGroup->AddNewObject(new Engine::Label(
            "michael \"mihu21\" huang", "pirulen.ttf", PlayScene::BlockSize/3,
            halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize*1.5;

    creditGroup->AddNewObject(new Engine::Label(
        "graphic Design", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 215, 0, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;

    creditGroup->AddNewObject(new Engine::Label(
        "Feivel \"anotherfei\" Kusuma", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;

    creditGroup->AddNewObject(new Engine::Label(
            "Wilson \"wilsonanthonyt\" Tang", "pirulen.ttf", PlayScene::BlockSize/3,
            halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    creditGroup->AddNewObject(new Engine::Image(
        "scoreboard/bryanmedal.png", w-PlayScene::BlockSize*2.5, currentY,
        PlayScene::BlockSize*3, PlayScene::BlockSize*4, 0.5, 0.5));
    currentY += PlayScene::BlockSize;
    creditGroup->AddNewObject(new Engine::Label(
            "michael \"mihu21\" huang", "pirulen.ttf", PlayScene::BlockSize/3,
            halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));

    creditGroup->AddNewObject(new Engine::Image(
            "scoreboard/arwenmedal.png", PlayScene::BlockSize*2.5, currentY,
            PlayScene::BlockSize*3, PlayScene::BlockSize*4, 0.5, 0.5));
    currentY += PlayScene::BlockSize*1.5;
    creditGroup->AddNewObject(new Engine::Label(
        "Story Developer", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 215, 0, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;

    creditGroup->AddNewObject(new Engine::Label(
        "Feivel \"anotherfei\" Kusuma", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;

    creditGroup->AddNewObject(new Engine::Label(
            "Wilson \"wilsonanthonyt\" Tang", "pirulen.ttf", PlayScene::BlockSize/3,
            halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;
    creditGroup->AddNewObject(new Engine::Label(
            "michael \"mihu21\" huang", "pirulen.ttf", PlayScene::BlockSize/3,
            halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize*1.5;

    creditGroup->AddNewObject(new Engine::Label(
        "Special Thanks", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 215, 0, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;
    creditGroup->AddNewObject(new Engine::Label(
        "Bryan", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    creditGroup->AddNewObject(new Engine::Image(
        "start/bryanreal.jpg", w-PlayScene::BlockSize*2.5, currentY,
        PlayScene::BlockSize*3, PlayScene::BlockSize*3, 0.5, 0.5));
    currentY += PlayScene::BlockSize;
    creditGroup->AddNewObject(new Engine::Label(
        "Arwen", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    creditGroup->AddNewObject(new Engine::Image(
        "start/arwenreal.jpg", PlayScene::BlockSize*2.5, currentY,
        PlayScene::BlockSize*3, PlayScene::BlockSize*3, 0.5, 0.5));
    currentY += PlayScene::BlockSize * 4;
    // Final thank you
    creditGroup->AddNewObject(new Engine::Label(
        "THANK YOU FOR PLAYING!", "pirulen.ttf", PlayScene::BlockSize/2,
        halfW, currentY, 255, 215, 0, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize * 5;
    creditGroup->AddNewObject(new Engine::Label(
    "You haven't seen the last of Ejojo...", "pirulen.ttf", PlayScene::BlockSize/3,
    halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += PlayScene::BlockSize;
    creditGroup->AddNewObject(new Engine::Label(
        "Something worse is coming...", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += h/2;
    creditGroup->AddNewObject(new Engine::Label(
        "To be continued...", "pirulen.ttf", PlayScene::BlockSize/3,
        halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
    currentY += h;
    creditGroup->AddNewObject(new Engine::Image(
        "play/doraemon.png", halfW, currentY,
        PlayScene::BlockSize*6, PlayScene::BlockSize*6, 0.5, 0));

    CreateUI();
    ResetScroll();
}

void CreditScene::Update(float deltaTime) {
    if (isScrolling) {
        scrollY -= scrollSpeed * deltaTime;

        // Check if all credits have passed the top
        float totalHeight = creditGroup->GetObjects().back()->Position.y + PlayScene::BlockSize*2;
        int screenHeight = Engine::GameEngine::GetInstance().GetScreenHeight();
        if (scrollY < -totalHeight) {
            Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
        }
    }

    UIGroup->Update(deltaTime);
}

void CreditScene::Draw() const {
    // Clear screen with dark background
    al_clear_to_color(al_map_rgb(10, 10, 20));

    // Set up transform for scrolling
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    al_translate_transform(&transform, 0, scrollY);
    al_use_transform(&transform);

    // Draw all credits
    creditGroup->Draw();

    // Reset transform
    al_identity_transform(&transform);
    al_use_transform(&transform);

    // Draw UI
    UIGroup->Draw();

    // Draw instructions using built-in font (no AddNewObject in Draw)
    ALLEGRO_FONT* font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", PlayScene::BlockSize/6).get();
    al_draw_text(
        font,
        al_map_rgb(200, 200, 200),
        Engine::GameEngine::GetInstance().GetScreenWidth()-PlayScene::BlockSize/4,
        PlayScene::BlockSize/4,
        ALLEGRO_ALIGN_RIGHT,
        "Press any key to fast forward"
    );
}

void CreditScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    IScene::Terminate();
}

void CreditScene::ResetScroll() {
    scrollY = Engine::GameEngine::GetInstance().GetScreenHeight();
}

void CreditScene::CreateUI() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;

    // Back button
    backButton = new Engine::ImageButton(
        "start/button.png",
        "stage-select/floor.png",
        w - PlayScene::BlockSize*2.5,
        h - PlayScene::BlockSize*1.5,
        PlayScene::BlockSize*2,
        PlayScene::BlockSize
    );
    backButton->SetOnClickCallback(std::bind(&CreditScene::BackOnClick, this, 0));
    UIGroup->AddNewControlObject(backButton);

    // Back button label
    UIGroup->AddNewObject(new Engine::Label(
        "Skip", "pirulen.ttf", PlayScene::BlockSize/3,
        w - PlayScene::BlockSize*1.5,
        h - PlayScene::BlockSize,
        10, 255, 255, 255, 0.5, 0.5));
}

void CreditScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard");
}

void CreditScene::OnKeyDown(int keyCode) {
    scrollSpeed=scrollSpeed*4;
}
void CreditScene::OnKeyUp(int keyCode) {
    scrollSpeed=scrollSpeed/4;
}