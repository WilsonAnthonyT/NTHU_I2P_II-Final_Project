#ifndef SCOREBOARDSCENE_HPP
#define SCOREBOARDSCENE_HPP
#include "PlayScene.hpp"
#include "Engine/IScene.hpp"

class ScoreboardScene final : public Engine::IScene {
private:
    struct saved {
        std::string name;
        int scores;
        std::string date_n_time;
    };

    std::ifstream ifs;
    std::string Name;
    float Spacing;
    int Page = 0;
    int MAXPAGES = 0;
    const int List_per_Page = 10;
    std::vector<saved> Data;

    Engine::Label *SortLabel;
    std::string Stype = "Score";
    int sortType = 0;
    void sortData(int type);

public:
    explicit ScoreboardScene() = default;
    void Initialize() override;
    void OnKeyDown(int keyCode) override;
    void Draw() const override;
    void Terminate() override;
    void CloseOnClick(int stage);
    void PageButtonOnClick(int stage);
    void SortOnClick(int stage);
};

#endif   // SCOREBOARDSCENE_HPP
