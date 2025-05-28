//
// Created by User on 11/05/2025.
//
#include "ShovelTool.hpp"

#include <iostream>
#include <ostream>

#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/AudioHelper.hpp"
#include "UI/Animation/DirtyEffect.hpp"

const int ShovelTool::Price = 0;

ShovelTool::ShovelTool(float x, float y) :
    Turret("play/better_shovels.png", "play/better_shovels.png", x, y, 0, Price, 0) {
    Target = nullptr;
}

void ShovelTool::OnMouseDown(int gridX, int gridY) {
    bool shovelSoundplayed = false;
    PlayScene *scene = Turret::getPlayScene();
    if (!scene) return;
    Turret *turret = scene->FindTurret(gridX, gridY);
    if (turret) {
        turret->GetObjectIterator()->first = false;
        scene->TowerGroup->RemoveObject(turret->GetObjectIterator());
        if (!shovelSoundplayed) {
            AudioHelper::PlayAudio("button_pressed.mp3");
            shovelSoundplayed = true;
        }
        scene->ClearTile(gridX, gridY);
        scene->EarnMoney(25 * turret->GetPrice() / 100);
    }
    else {
        Engine::Sprite *sprite;
        scene->GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 0.5, gridX * PlayScene::BlockSize + PlayScene::BlockSize / 2, gridY * PlayScene::BlockSize + PlayScene::BlockSize / 2));
        sprite->Rotation = 0;
    }
    //scene->UIGroup->RemoveObject(GetObjectIterator());
    std::cout << Turret::GetPrice() << std::endl;
}

void ShovelTool::CreateBullet() {}

