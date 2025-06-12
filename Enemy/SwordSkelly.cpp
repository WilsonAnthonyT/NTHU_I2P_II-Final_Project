//
// Created by MiHu on 6/12/2025.
//

#include "SwordSkelly.h"

#include "Scene/PlayScene.hpp"

SwordSkelly::SwordSkelly(int x, int y) : Enemy("play/skeletonsword.png", x, y, 100, PlayScene::BlockSize * 1.25f, 10, 5, 5, 10) {
}
