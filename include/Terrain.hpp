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
        chunks(50),
        weights(0.0f, 0.2f, 0.5f, 1.0f),
        colors(vec3(0.184f, 0.694f, 0.831f),
               vec3(0.357f, 0.6f, 0.369f),
               vec3(0.58f, 0.49f, 0.388f),
               vec3(0.969f, 1.0f, 0.996f)),
        freqFnoise(0.001f), ampFnoise(0.025f), octFnoise(1), seedFnoise(123),
        freqAnoise(0.01f), ampAnoise(50.0f), octAnoise(1), seedAnoise(789),
        isFogActive(true) { }

    float chunkSize;
    int chunks;

    float weights[4];
    vec3 colors[4];

    float freqFnoise;
    float ampFnoise;
    int octFnoise;
    int seedFnoise;

    float freqAnoise;
    float ampAnoise;
    int octAnoise;
    int seedAnoise;

    bool isFogActive;
};