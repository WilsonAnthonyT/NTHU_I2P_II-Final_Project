#ifndef CREDITSCENE_H
#define CREDITSCENE_H

#include "Engine/IScene.hpp"
#include "PlayScene.hpp"
#include <allegro5/allegro.h>
#include <vector>

class CreditScene : public Engine::IScene {
private:
    float scrollY;
    float scrollSpeed;
    bool isScrolling;
    Engine::Group* creditGroup;
    Engine::Group* UIGroup;
    Engine::ImageButton* backButton;

public:
    CreditScene();
    ~CreditScene() override;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void Terminate() override;

    void ResetScroll();
    void CreateUI();
    void BackOnClick(int stage);
    void OnKeyDown(int keyCode) override;
};

#endif // CREDITSCENE_H