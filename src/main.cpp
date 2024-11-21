/***************************************************************************************************
 * @file  main.cpp
 * @brief Contains the main program of the engine
 **************************************************************************************************/

#include "Application.hpp"
#include "WhoAmI.hpp"

#include <iostream>
#include <stdexcept>

Application app;

int main() {
    try {
        #if defined I_AM_MINAS
            app.runMinas();
        #else
            app.run();
        #endif
    } catch(const std::exception& exception) {
        std::cerr << "ERROR : " << exception.what() << '\n';
        return -1;
    }

    return 0;
}