#ifndef LEADERBOARDSCENE_HPP
#define LEADERBOARDSCENE_HPP
#include "Engine/IScene.hpp"

class LeaderboardScene final : public Engine::IScene {
private:
    std::string Name, dateNtime;
    int tick;
    const int maxChar = 10;
    bool enterPressed = false;

public:
    explicit LeaderboardScene() = default;
    void Initialize() override;
    void OnKeyDown(int keyCode) override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void Terminate() override;
    void DontSaveOnClick(int stage);

};

#endif   // LEADERBOARDSCENE_HPP
