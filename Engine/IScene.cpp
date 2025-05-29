#include "IScene.hpp"

#include <iostream>
#include <ostream>
#include <stdint.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "GameEngine.hpp"
#include "Scene/PlayScene.hpp"

namespace Engine {

    void IScene::Terminate() {
        Clear();
    }
    void IScene::Draw() const {
        al_clear_to_color(al_map_rgb(0, 0, 0));
        Group::Draw();

        if (DebugMode) {
            const int block = Engine::GameEngine::GetInstance().GetScreenWidth() / 16;
            const int w = Engine::GameEngine::GetInstance().GetScreenWidth();
            const int h = Engine::GameEngine::GetInstance().GetScreenHeight();
            for (int i = 0; i < h; i += block) {
                for (int j = 0; j < w; j += block) {
                    al_draw_rectangle(j, i, j + block, i + block, al_map_rgb(255,255,255), 2.0);
                }
            }
        }
    }
    void IScene::OnKeyDown(int keyCode) {
        if (keyCode == ALLEGRO_KEY_TAB) {
            DebugMode = !DebugMode;
            std::cout << DebugMode << std::endl;
            int screenheight = Engine::GameEngine::GetInstance().GetScreenHeight();
            std::cout << screenheight << std::endl;
        }
    }

}
