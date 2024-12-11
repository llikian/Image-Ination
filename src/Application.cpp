/***************************************************************************************************
 * @file  Application.cpp
 * @brief Implementation of the Application class
 **************************************************************************************************/

#include "Application.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"
#include "mesh/meshes.hpp"

Application::Application()
    : window(this),
      time(0.0f), delta(0.0f),
      lightDirection(2.0f, 2.0f, 0.0f),
      wireframe(false), cullface(true), isCursorVisible(false),
      sTerrain(nullptr), sWater(nullptr), sNWater(nullptr), sClouds(nullptr),
      chunkSize(32.0f), chunks(128),
      projection(perspective(M_PI_4f, window.getRatio(), 0.1f, 2.0f * chunkSize * chunks)),
      camera(vec3(0.0f, 20.0f, 0.0f)),
      cameraPos(camera.getPositionReference()),
      grid(Meshes::tessGrid(chunkSize * chunks, chunks)),
      plane(Meshes::chunk()), screen(Meshes::screen()),
      texRock("data/rock.jpg"), texRockSmooth("data/rock_smooth.jpg"), texGrass("data/grass.jpg"),
      texGrassDark("data/grass_dark.png"), texSnow("data/snow.png") {

    /**** ImGui ****/
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().IniFilename = "lib/imgui/imgui.ini";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 420");

    /**** Shaders ****/
    std::string paths[4]{
        "shaders/terrain/terrain.vert",
        "shaders/terrain/terrain.tesc",
        "shaders/terrain/terrain.tese",
        "shaders/terrain/terrain.frag"
    };
    sTerrain = new Shader(paths, 4, "Terrain");

    paths[2] = "shaders/noise_water/noise_water.tese";
    paths[3] = "shaders/noise_water/noise_water.frag";
    sNWater = new Shader(paths, 4, "Noise Water");

    paths[0] = "shaders/water/water.vert";
    paths[1] = "shaders/water/water.frag";
    sWater = new Shader(paths, 2, "Water");

    paths[0] = "shaders/clouds/clouds.vert";
    paths[1] = "shaders/clouds/clouds.frag";
    sClouds = new Shader(paths, 2, "Clouds");

    paths[0] = "shaders/clouds/rain.vert";
    paths[1] = "shaders/clouds/rain.frag";
    sRain = new Shader(paths, 2, "Rain");

    /**** Textures ****/
    sTerrain->use();

    sTerrain->setUniform("texRock", 0);
    texRock.bind(0);
    sTerrain->setUniform("texRockSmooth", 1);
    texRockSmooth.bind(1);
    sTerrain->setUniform("texGrass", 2);
    texGrass.bind(2);
    sTerrain->setUniform("texGrassDark", 3);
    texGrassDark.bind(3);
    sTerrain->setUniform("texSnow", 4);
    texSnow.bind(4);
}

Application::~Application() {
    delete sTerrain;
    delete sWater;
    delete sNWater;
    delete sClouds;
    delete sRain;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::run() {
    while(!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        handleEvents();
        updateVariables();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /**** Background & Clouds ****/
        sClouds->use();
        updateCloudsUniforms();
        drawClouds();

//        sRain->use();
//        updateRainUniforms();
//        drawClouds();

        /**** Terrain ****/
        sTerrain->use();
        updateTerrainUniforms();
        grid.draw();

        /**** Noise Water ****/
        sNWater->use();
        updateNoiseWaterUniforms();
        grid.draw();

        /**** Debug ImGui Window ****/
        debugWindow();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void Application::setWindowSize(int width, int height) {
    window.updateSize(width, height);
    projection[0][0] = 1.0f / (tanf(M_PI_4f / 2.0f) * window.getRatio());
}

void Application::handleKeyCallback(int key, int action, int /* mods */) {
    if(action == GLFW_PRESS) {
        keys[key] = true;
    } else if(action == GLFW_RELEASE) {
        keys[key] = false;
    }
}

void Application::handleCursorPositionEvent(float xPos, float yPos) {
    if(!isCursorVisible) {
        camera.look(vec2(xPos - mousePos.x, yPos - mousePos.y));
    }

    mousePos.x = xPos;
    mousePos.y = yPos;
}

void Application::handleEvents() {
    glfwPollEvents();
    handleKeyboardEvents();
}

void Application::handleKeyboardEvents() {
    for(const auto& [key, isKeyActive]: keys) {
        if(isKeyActive) {
            switch(key) {
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(window, true);
                    break;
                case GLFW_KEY_TAB:
                    glfwSetInputMode(window, GLFW_CURSOR, isCursorVisible ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                    isCursorVisible = !isCursorVisible;

                    keys[key] = false;
                    break;
                case GLFW_KEY_Z:
                    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_FILL : GL_LINE);
                    wireframe = !wireframe;

                    keys[key] = false;
                    break;
                case GLFW_KEY_C:
                    (cullface ? glDisable : glEnable)(GL_CULL_FACE);
                    cullface = !cullface;

                    keys[key] = false;
                    break;
                case GLFW_KEY_W:
                    camera.move(CameraControls::forward, delta);
                    break;
                case GLFW_KEY_S:
                    camera.move(CameraControls::backward, delta);
                    break;
                case GLFW_KEY_A:
                    camera.move(CameraControls::left, delta);
                    break;
                case GLFW_KEY_D:
                    camera.move(CameraControls::right, delta);
                    break;
                case GLFW_KEY_SPACE:
                    camera.move(CameraControls::upward, delta);
                    break;
                case GLFW_KEY_LEFT_SHIFT:
                    camera.move(CameraControls::downward, delta);
                    break;
            }
        }
    }
}

void Application::updateVariables() {
    delta = glfwGetTime() - time;
    time = glfwGetTime();
    vpMatrix = projection * camera.getViewMatrix();
    cameraChunk.x = floor(0.5f + cameraPos.x / chunkSize);
    cameraChunk.y = floor(0.5f + cameraPos.z / chunkSize);
}

void Application::debugWindow() {
    ImGui::Begin("Debug");
    ImGui::Text("%d FPS | %.2fms/frame", static_cast<int>(1.0f / delta), 1000.0f * delta);
    ImGui::Text("Time: %.4fs", time);
    ImGui::Text("Position: (%.2f ; %.2f ; %.2f)", cameraPos.x, cameraPos.y, cameraPos.z);
    ImGui::Text("Chunk: (%.0f ; %.0f)", cameraChunk.x, cameraChunk.y);
    ImGui::InputFloat("Camera Speed", &camera.movementSpeed);
    ImGui::End();
}

void Application::updateTerrainUniforms() {
    sTerrain->setUniform("vpMatrix", vpMatrix);
    sTerrain->setUniform("cameraPos", cameraPos);
    sTerrain->setUniform("cameraChunk", cameraChunk);
    sTerrain->setUniform("chunkSize", chunkSize);
    sTerrain->setUniform("totalTerrainWidth", chunks * chunkSize / 2.0f);
    sTerrain->setUniform("lightDirection", lightDirection);
}

void Application::drawWater() {
    if(wireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

    screen.draw();

    if(wireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
}

void Application::updateNoiseWaterUniforms() {
    sNWater->setUniform("vpMatrix", vpMatrix);
    sNWater->setUniform("cameraPos", cameraPos);
    sNWater->setUniform("cameraChunk", cameraChunk);
    sNWater->setUniform("chunkSize", chunkSize);
    sNWater->setUniform("totalTerrainWidth", chunks * chunkSize / 2.0f);
    sNWater->setUniform("lightDirection", lightDirection);
    sNWater->setUniform("time", time);
}

void Application::updateWaterUniforms() {
    sWater->setUniform("resolution", window.getResolution());
    sWater->setUniform("cameraPos", cameraPos);
    sWater->setUniform("cameraFront", camera.getDirection());
    sWater->setUniform("cameraRight", camera.getRight());
    sWater->setUniform("cameraUp", camera.getUp());
    sWater->setUniform("time", time);
}

void Application::drawClouds() {
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    if(wireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

    screen.draw();

    if(wireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}

void Application::drawRain() {

    if(wireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

    screen.draw();

    if(wireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }

}

void Application::updateCloudsUniforms() {
    sClouds->setUniform("resolution", window.getResolution());
    sClouds->setUniform("cameraPosition", cameraPos);
    sClouds->setUniform("cameraFront", camera.getDirection());
    sClouds->setUniform("cameraRight", camera.getRight());
    sClouds->setUniform("cameraUp", camera.getUp());
    sClouds->setUniform("time", time);
}

void Application::updateRainUniforms() {
    sRain->setUniform("resolution", window.getResolution());
    sRain->setUniform("time", time);

}