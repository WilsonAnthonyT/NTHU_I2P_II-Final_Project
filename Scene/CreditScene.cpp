#include "CreditScene.h"
#include "Engine/GameEngine.hpp"
#include <allegro5/allegro_primitives.h>

#include "UI/Component/Label.hpp"

CreditScene::CreditScene() :
    scrollY(0),
    scrollSpeed(120),
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
    currentY += PlayScene::BlockSize;
    creditGroup->AddNewObject(new Engine::Label(
            "michael \"mihu21\" huang", "pirulen.ttf", PlayScene::BlockSize/3,
            halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
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
    currentY += PlayScene::BlockSize;
    creditGroup->AddNewObject(new Engine::Label(
            "michael \"mihu21\" huang", "pirulen.ttf", PlayScene::BlockSize/3,
            halfW, currentY, 255, 255, 255, 255, 0.5, 0.5));
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
    currentY += PlayScene::BlockSize * 2.f;
    // Final thank you
    creditGroup->AddNewObject(new Engine::Label(
        "THANK YOU FOR PLAYING!", "pirulen.ttf", PlayScene::BlockSize/2,
        halfW, currentY, 255, 215, 0, 255, 0.5, 0.5));

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
            ResetScroll();
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
    const char* instruction = isScrolling ? "Press any key to pause/resume" : "Scrolling paused - press any key to resume";
    al_draw_text(
        al_create_builtin_font(),
        al_map_rgb(200, 200, 200),
        Engine::GameEngine::GetInstance().GetScreenWidth()/2,
        Engine::GameEngine::GetInstance().GetScreenHeight() - PlayScene::BlockSize,
        ALLEGRO_ALIGN_CENTER,
        instruction
    );
}

void CreditScene::Terminate() {
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
        "stage-select/dirt.png",
        "stage-select/floor.png",
        w - PlayScene::BlockSize*4,
        h - PlayScene::BlockSize*2,
        PlayScene::BlockSize*3.5,
        PlayScene::BlockSize*1.5
    );
    backButton->SetOnClickCallback(std::bind(&CreditScene::BackOnClick, this, 0));
    UIGroup->AddNewControlObject(backButton);

    // Back button label
    UIGroup->AddNewObject(new Engine::Label(
        "Back", "pirulen.ttf", PlayScene::BlockSize/3,
        w - PlayScene::BlockSize*2.25,
        h - PlayScene::BlockSize*1.75,
        0, 0, 0, 255, 0.5, 0.5));
}

void CreditScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("start");
}

void CreditScene::OnKeyDown(int keyCode) {
    isScrolling = !isScrolling;
}