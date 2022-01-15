#include "motor/Application.hpp"
#include "motor/logging.hpp"
#include <game/ShmupGame.hpp>

int main()
{
    LOG("Launching IT517G...");

    Application::InitAndCreateWindow(ShmupGame::width, ShmupGame::height, "Fördjupningsarbete i spelutveckling");
    Application::CreateEntity<ShmupGame>();
    Application::RunMainLoop();
    Application::CleanUp();

    return 0;
}
