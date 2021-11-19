#include <cstdlib>
#include "Application.hpp"
#include "logging.hpp"

int main()
{
    LOG("Launching IT517G");

    Application::InitAndCreateWindow(320, 180, "Fördjupningsarbete i spelutveckling");
    Application::RunMainLoop();
    Application::CleanUp();

    return 0;
}
