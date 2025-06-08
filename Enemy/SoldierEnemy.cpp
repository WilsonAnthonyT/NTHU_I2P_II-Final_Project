#include <string>

#include "SoldierEnemy.hpp"

#include "Scene/PlayScene.hpp"

// TODO HACKATHON-3 (1/3): You can imitate the 2 files: 'SoldierEnemy.hpp', 'SoldierEnemy.cpp' to create a new enemy.
SoldierEnemy::SoldierEnemy(int x, int y) : Enemy("play/Demon.png", x, y, 100, PlayScene::BlockSize * 1.25f, 10, 5, 5, 10) {
}
