#include "Application.hpp"
#include "logging.hpp"

int main()
{
    LOG("Launching IT517G");

    Application::InitAndCreateWindow(1024, 768, "Fördjupningsarbete i spelutveckling");
    Application::RunMainLoop();
    Application::CleanUp();

    return 0;
}
