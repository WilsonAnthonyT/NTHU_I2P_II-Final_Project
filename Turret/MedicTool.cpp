//
// Created by User on 14/05/2025.
//

#include "MedicTool.hpp"

#include <iostream>
#include <ostream>

#include "Scene/PlayScene.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/AudioHelper.hpp"

const int MedicTool::Price = 250;

MedicTool::MedicTool(float x, float y) :
    Turret("play/medkit.png", "play/medkit.png", x, y, 0, Price, 0) {
    Target = nullptr;
}

void MedicTool::OnMouseDown(int gridX, int gridY) {
    PlayScene *scene = getPlayScene();
    if (!scene) return;
    //scene->UIGroup->RemoveObject(GetObjectIterator());
    if (scene->getLives() >= scene->maxLives) return;
    scene->setLives(scene->maxLives - scene->getLives());
    AudioHelper::PlayAudio("HealingSFX.mp3");
    std::cout << "Heal Successful" << std::endl;
}

void MedicTool::CreateBullet() {}
