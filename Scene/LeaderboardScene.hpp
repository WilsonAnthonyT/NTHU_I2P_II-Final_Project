#ifndef LEADERBOARDSCENE_HPP
#define LEADERBOARDSCENE_HPP
#include "Engine/IScene.hpp"

class LeaderboardScene final : public Engine::IScene {
private:
    std::ofstream ofs;
    std::string Name, dateNtime;
    int tick;
    int score;
    const int maxChar = 14;
    bool enterPressed = false;

public:
    static int val;
    explicit LeaderboardScene() = default;
    void Initialize() override;
    void OnKeyDown(int keyCode) override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void Terminate() override;
    void DontSaveOnClick(int stage);
    void setScore(){score = val;}
};

#endif   // LEADERBOARDSCENE_HPP
