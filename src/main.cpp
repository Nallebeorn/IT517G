#include <cstdlib>

#include "motor/Application.hpp"
#include "motor/logging.hpp"

int main()
{
    LOG("Launching IT517G...");

    Application::InitAndCreateWindow(320, 180, "Fördjupningsarbete i spelutveckling");
    Application::RunMainLoop();
    Application::CleanUp();

    return 0;
}
