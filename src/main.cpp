/***************************************************************************************************
 * @file  main.cpp
 * @brief Contains the main program of the engine
 **************************************************************************************************/

#include "Application.hpp"
#include "WhoAmI.hpp"

#include <iostream>
#include <stdexcept>

Application app(initLibraries());

int main() {
    try {
        #if defined I_AM_MINAS
            app.runMinas();
        #elif defined I_AM_KILLIAN
            app.runKillian();
        #else
            app.runRaph();
        #endif
    } catch(const std::exception& exception) {
        std::cerr << "ERROR : " << exception.what() << '\n';
        return -1;
    }

    return 0;
}