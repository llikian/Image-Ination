/***************************************************************************************************
 * @file  Terrain.hpp
 * @brief Declaration of the Terrain struct
 **************************************************************************************************/

#pragma once

#include <glm/vec3.hpp>

using namespace glm;

/**
 * @struct Terrain
 * @brief Contains all the data needed to render the terrain.
 */
struct Terrain {
    Terrain() :
        chunkSize(32.0f),
        chunks(128),
        isFogActive(true) { }

    const float chunkSize;
    const int chunks;

    bool isFogActive;
};