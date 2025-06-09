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

        if (backgroundIMG) {
            const int screenW = Engine::GameEngine::GetInstance().GetScreenWidth();
            const int screenH = Engine::GameEngine::GetInstance().GetScreenHeight();

            // Get background dimensions
            const int bgWidth = al_get_bitmap_width(backgroundIMG.get());
            const int bgHeight = al_get_bitmap_height(backgroundIMG.get());

            // Draw the background (stretched vertically, looped horizontally)
            for (float x = 0; x < PlayScene::MapWidth * PlayScene::BlockSize; x += bgWidth) {
                al_draw_scaled_bitmap(
                    backgroundIMG.get(),    // Source bitmap
                    0, 0,                  // Source X, Y
                    bgWidth, bgHeight,      // Source width, height
                    x, PlayScene::Camera.y,                  // Destination X, Y
                    bgWidth, screenH,      // Destination width, height (stretch vertically)
                    0                      // Flags (0 = no blending)
                );
            }
        }

        if (DebugMode) {
            const int block = Engine::GameEngine::GetInstance().GetScreenWidth() / 16;
            const int w = Engine::GameEngine::GetInstance().GetScreenWidth();
            const int h = Engine::GameEngine::GetInstance().GetScreenHeight();
            for (int i = 0; i < h; i += block) {
                for (int j = 0; j < w; j += block) {
                    al_draw_rectangle(j, i, j + block, i + block, al_map_rgba(155,155,155, 100), 2.0);
                }
            }
        }
        Group::Draw();
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
