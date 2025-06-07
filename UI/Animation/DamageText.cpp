//
// Created by User on 07/06/2025.
//

#include "DamageText.h"
// UI/DamageText.cpp
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include "Engine/Resources.hpp"

DamageText::DamageText(float x, float y, const std::string& text, ALLEGRO_COLOR color)
    : IObject(x, y), text(text), color(color) {
    timeLeft = 0.5f;       // visible for 0.5 seconds
    riseSpeed = 60.0f;     // pixels per second upward
    removeDT = false;
}

void DamageText::Update(float deltaTime) {
    Position.y -= riseSpeed * deltaTime;
    timeLeft -= deltaTime;
    if (timeLeft <= 0) {
        removeDT = true;
    }
}

void DamageText::Draw() const {
    auto fontPtr = Engine::Resources::GetInstance().GetFont("pirulen.ttf", 32);
    ALLEGRO_FONT* font = fontPtr.get();  // Extract raw pointer
    al_draw_text(font, color, Position.x, Position.y, ALLEGRO_ALIGN_CENTER, text.c_str());
}
