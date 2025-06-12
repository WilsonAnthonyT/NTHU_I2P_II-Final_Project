#include <string>

#include "SoldierEnemy.hpp"

#include "Scene/PlayScene.hpp"

SoldierEnemy::SoldierEnemy(int x, int y) : Enemy("play/Demon.png", x, y, 100, PlayScene::BlockSize * 1.25f, 10, 5, 5, 10) {
}
