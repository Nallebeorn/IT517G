#pragma once

#include <motor/Entity.hpp>

class ShmupGame : public Entity
{
public:
    ShmupGame() {};
    ~ShmupGame();

    inline static ShmupGame *Get() { return instance; };
    static constexpr int32 width = 320;
    static constexpr int32 height = 180;
    uint32 score = 0;

private:
    float enemySpawnTimer = 0;
    bool gameOver = false;
    inline static ShmupGame *instance = nullptr;

public:
    void Create();
    virtual void Update() override;
    void EndGame();

private
:
    void DrawParallax(const char *sprite, float speed);
};
