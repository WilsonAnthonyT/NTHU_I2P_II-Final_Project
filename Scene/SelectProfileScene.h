//
// Created by MiHu on 6/11/2025.
//

#ifndef SELECTPROFILESCENE_H
#define SELECTPROFILESCENE_H
#include "Engine/IScene.hpp"


class SelectProfileScene final : public Engine::IScene {

    const int MAXprofile = 3;
    static int profileID;

    struct ProfileData {
        std::string Name;
        std::string Created;
        std::string Last_Played;
        std::string Duration;
        std::string Stage;
    };

public:
    explicit SelectProfileScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int);
    void BackOnClick();

    static struct textData {
        int HP_1;
        int HP_2;

        textData() : HP_1(0), HP_2(0) {};
    };

    static textData* ReadProfileData(const int);
    static void WriteProfileData(const int, textData*);
    static int getProfileID() {return profileID;}

    // void BGMSlideOnValueChanged(float value);
    // void SFXSlideOnValueChanged(float value);
};



#endif //SELECTPROFILESCENE_H
