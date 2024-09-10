// ###################################################################
//  IMPORTANTE SEGUIR LA FACHADA Y TENER UNA ESTRUCTURA CLARA Y OPTIMA
// ###################################################################

#include <stdexcept>
#include <iostream>
// Device and Engine
#include <engines/device.hpp>
#include <game/states/menus/MainMenu.hpp>

int main()
{
    try
    {
        SGDevice device{1280, 720};
        StateManager states{};
        states.addState<MainMenu>(states, device);
        while (states.alive() && device.isRun())
            states.update();
    }
    catch (...)
    {
        std::cerr << "\n*** EXCEPTION ***\n";
    }

    return 0;
}
