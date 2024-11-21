/***************************************************************************************************
 * @file  Application.hpp
 * @brief Declaration of the Application class
 **************************************************************************************************/

#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <glm/vec2.hpp>

#include "Camera.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Window.hpp"
#include "mesh/Mesh.hpp"

using namespace glm;

/**
 * @class Application
 * @brief The core of the engine.
 */
class Application {
public:
    /**** Constructor & Destructor ****/

    /**
     * @brief Initializes the window and sets the default value of all member variables and constants.
     */
    Application();

    /**
     * @brief Frees all allocated memory.
     */
    ~Application();

    /**** Public Methods ****/

    /**
     * @brief Contains the main loop for Minas.
     */
    void runMinas();

    /**
     * @brief Contains the main loop.
     */
    void run();

    /**
     * @brief Sets the width and height of the GLFW window.
     * @param width The new width of the window.
     * @param height The new height of the window.
     */
    void setWindowSize(int width, int height);

    /**
     * @brief Handles what happens when a key is pressed.
     * @param key
     * @param action
     * @param mods
     */
    void handleKeyCallback(int key, int action, int mods);

    /**
     * @brief Handles the event where the cursor is moved.
     * @param xPos The new x position of the cursor.
     * @param yPos The new y position of the cursor.
     */
    void handleCursorPositionEvent(float xPos, float yPos);

private:
    /**** Private Methods ****/

    /**
     * @brief Polls and handles events with glfw.
     */
    void handleEvents();

    /**
     * @brief Handles keyboard events.
     */
    void handleKeyboardEvents();

    /**
     * @brief Updates the member variables that change every frame.
     */
    void updateVariables();

    /**
     * @brief Configures the ImGui window to show debug information.
     */
    void debugWindow();

    /**
     * @brief Updates all of the terrain's shader program's uniforms.
     */
    void updateTerrainUniforms();

    /**
     * @brief Draws the water.
     */
    void drawWater();

    /**
     * @brief Updates all of the water's shader program's uniforms.
     */
    void updateWaterUniforms();

    /**
     * @brief Updates all of the noise water's shader program's uniforms.
     */
    void updateNoiseWaterUniforms();

    /**
     * @brief Draws the clouds.
     */
    void drawClouds();

    /**
     * @brief Updates all of the clouds's shader program's uniforms.
     */
    void updateCloudsUniforms();

    /**** Variables & Constants ****/
    Window window; ///< The GLFW window.

    std::unordered_map<int, bool> keys; ///< Map of the current state of keys.

    vec2 mousePos; ///< The position of the mouse on the screen.

    float time;  ///< The current time in seconds;
    float delta; ///< The time difference between this frame and the previous in seconds.

    vec3 lightDirection; ///< The direction of the global lighting.

    bool wireframe;       ///< Whether to display in wireframe mode.
    bool cullface;        ///< Whether face culling is activated.
    bool isCursorVisible; ///< Whether the cursor is currently visible.

    Shader* sTerrain; ///< The shader program for rendering the terrain.
    Shader* sWater;   ///< The shader program for rendering the water.
    Shader* sNWater;  ///< The shader program for rendering the water made with noise.
    Shader* sClouds;  ///< The shader program for rendering the clouds.

    const float chunkSize; ///< The side length of a chunk.
    const int chunks; ///< The side length of the chunk grid.

    mat4 projection; ///< The projection matrix.
    mat4 vpMatrix;   ///< The view/projection matrix.

    Camera camera;         ///< A first person camera to move around the scene.
    const vec3& cameraPos; ///< The camera's position.
    vec2 cameraChunk;      ///< The chunk the camera is in.

    Mesh grid;    ///< Mesh for a grid. Used to render the terrain.
    Mesh plane;   ///< Mesh for a plane. Used to render a chunk.
    Mesh screen;  ///< Mesh for a screen. Used to render the clouds.

    Texture texRock;       ///< Tileable rocky texture.
    Texture texRockSmooth; ///< Smoother tileable rocky texture.
    Texture texGrass;      ///< Tileable grass texture.
    Texture texGrassDark;  ///< Darker tileable grass texture.
    Texture texSnow;       ///< Tileable snow texture.
};