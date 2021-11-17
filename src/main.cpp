#include <cstdlib>
#include "Motor.hpp"
#include "logging.hpp"

int main()
{
    LOG("Launching IT517G");

    Motor::InitAndCreateWindow(1024, 768, "Fördjupningsarbete i spelutveckling");
    Motor::RunMainLoop();
    Motor::CleanUp();

    return 0;
}
