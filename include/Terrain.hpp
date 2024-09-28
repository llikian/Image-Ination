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
        chunkSize(8.0f),
        chunks(200),
        tesselationFactor(8.0f),
        weights(0.0f, 0.2f, 0.5f, 1.0f),
        colors(
            vec3(0.184f, 0.694f, 0.831f),
            vec3(0.357f, 0.6f, 0.369f),
            vec3(0.58f, 0.49f, 0.388f),
            vec3(0.969f, 1.0f, 0.996f)
        ) { }

    float chunkSize;
    int chunks;
    float tesselationFactor;

    float weights[4];
    vec3 colors[4];
};