#include <cstdlib>

#include "motor/Application.hpp"
#include "motor/logging.hpp"
#include "game/Player.hpp"

int main()
{
    LOG("Launching IT517G...");

    Application::InitAndCreateWindow(320, 180, "Fördjupningsarbete i spelutveckling");
    Application::CreateEntity<Player>();
    Application::RunMainLoop();
    Application::CleanUp();

    return 0;
}
