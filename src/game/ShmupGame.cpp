#include "ShmupGame.hpp"

#include <motor/Application.hpp>

#include <motor/Input.hpp>
#include <motor/Random.hpp>
#include <motor/Time.hpp>
#include "Enemy.hpp"
#include "Player.hpp"

ShmupGame::~ShmupGame()
{
    if (ShmupGame::instance == this)
    {
        ShmupGame::instance = nullptr;
    }
}

void ShmupGame::Create()
{
    Application::CreateEntity<Player>();
    score = 21781;
    ShmupGame::instance = this;
}

void ShmupGame::Update()
{
    DrawParallax("Sky", 5.0f);
    DrawParallax("Mountains", 30.0f);
    DrawParallax("Forest", 80.0f);

    Gfx::DrawSprite("UIBar", ShmupGame::width / 2, 10);

    if (gameOver)
    {
        Gfx::DrawSprite("GameOver", width / 2, height / 2);

        if (Input::WasInputJustReleased(GLFW_KEY_ENTER))
        {
            Application::DestroyAllEntities();
            Application::CreateEntity<ShmupGame>();
        }
    }
    else
    {
        enemySpawnTimer -= Time::Delta();
        if (enemySpawnTimer <= 0)
        {
            enemySpawnTimer = Random::Range(0.25f, 1.0f);
            Enemy *enemy = Application::CreateEntity<Enemy>();
            enemy->x = ShmupGame::width + 40;
            enemy->y = Random::Range(0, ShmupGame::height);
        }
    }

}

void ShmupGame::EndGame()
{
    gameOver = true;
}

void ShmupGame::DrawParallax(const char *sprite, float speed)
{
    constexpr int32 bgWidth = 270;
    constexpr int32 bgHeight = 160;
    constexpr int32 bgY = ShmupGame::height - bgHeight + bgHeight / 2;

    float t = Time::AtFrameBegin();
    float offset = -t * speed;
    int32 offsetWrapped = ((int32)offset % bgWidth + bgWidth) % bgWidth;
    offsetWrapped -= bgWidth / 2;

    Gfx::DrawSprite(sprite, offsetWrapped, bgY);
    Gfx::DrawSprite(sprite, offsetWrapped + bgWidth, bgY);
    Gfx::DrawSprite(sprite, offsetWrapped + bgWidth * 2, bgY);
}
