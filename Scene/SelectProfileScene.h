//
// Created by MiHu on 6/11/2025.
//

#ifndef SELECTPROFILESCENE_H
#define SELECTPROFILESCENE_H
#include "Engine/IScene.hpp"


class SelectProfileScene final : public Engine::IScene {

    static constexpr int MAXprofile = 4;
    static int profileID;

public:
    struct ProfileData {
        std::string Name;
        std::string Created;
        std::string Last_Played;
        std::string Duration;
        std::string Stage;
    };
    static std::vector<ProfileData> playerData;

    explicit SelectProfileScene() = default;
    void Initialize() override;
    void Terminate() override;
    void PlayOnClick(int);
    void RemoveOnClick(int);
    void BackOnClick();

    struct textData {
        int level;
    };

    //static textData* ReadProfileData(const int);
    static void WriteProfileData(textData*);
    static int getProfileID() {return profileID;}
    static int getMAXprofile() {return MAXprofile;}
    static bool isSaved;

    // void BGMSlideOnValueChanged(float value);
    // void SFXSlideOnValueChanged(float value);
};



#endif //SELECTPROFILESCENE_H
