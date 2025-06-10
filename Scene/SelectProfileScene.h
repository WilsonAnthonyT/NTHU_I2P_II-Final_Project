//
// Created by MiHu on 6/11/2025.
//

#ifndef SELECTPROFILESCENE_H
#define SELECTPROFILESCENE_H
#include "Engine/IScene.hpp"


class SelectProfileScene final : public Engine::IScene {
private:
protected:
public:
    explicit SelectProfileScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int stage);
    void BackOnClick();

    void BGMSlideOnValueChanged(float value);
    void SFXSlideOnValueChanged(float value);
};



#endif //SELECTPROFILESCENE_H
