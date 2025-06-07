// UI/DamageText.hpp
#pragma once
#include "DirtyEffect.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Scene/PlayScene.hpp"
#include <string>

class DamageText : public Engine::IObject {
public:
    float timeLeft;
    float riseSpeed;
    bool removeDT;
    std::string text;
    ALLEGRO_COLOR color;

    DamageText(float x, float y, const std::string& text, ALLEGRO_COLOR color);

    void Update(float deltaTime) override;
    void Draw() const override;
};
