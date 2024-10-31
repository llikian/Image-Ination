/***************************************************************************************************
 * @file  Application.cpp
 * @brief Implementation of the Application class
 **************************************************************************************************/

#include "Application.hpp"

#include "callbacks.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "mesh/meshes.hpp"

Window initLibraries() {
    Window window{nullptr, 1600, 800};

    /**** GLFW ****/
    if(!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window.window = glfwCreateWindow(window.width, window.height, "Image-Ination", nullptr,
                                     nullptr);
    if(!window.window) {
        throw std::runtime_error("Failed to create window.");
    }

    glfwMakeContextCurrent(window.window);
    glfwMaximizeWindow(window.window);
    glfwSetInputMode(window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(0);

    /**** Actual Width / Height ****/
    int w, h;
    glfwGetWindowSize(window.window, &w, &h);
    window.width = w;
    window.height = h;

    /**** GLFW Callbacks ****/
    glfwSetWindowSizeCallback(window.window, windowSizeCallback);
    glfwSetFramebufferSizeCallback(window.window, frameBufferSizeCallback);
    glfwSetKeyCallback(window.window, keyCallback);
    glfwSetCursorPosCallback(window.window, cursorPositionCallback);

    /**** GLAD ****/
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD.");
    }

    /**** OpenGL ****/
    glViewport(0, 0, window.width, window.height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glActiveTexture(GL_TEXTURE0);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    // Sets the default texture to a plain white color
    const unsigned char white[3]{255, 255, 255};
    glBindTexture(GL_TEXTURE_2D, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white);

    /**** ImGui ****/
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().IniFilename = "lib/imgui/imgui.ini";
    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    return window;
}

Application::Application(Window window)
    : window(window.window), width(window.width), height(window.height),
      mousePos(width / 2.0f, height / 2.0f),
      time(0.0f), delta(0.0f),
      lightDirection(2.0f, 2.0f, 0.0f),
      wireframe(false), cullface(true), isCursorVisible(false),
      sTerrain(nullptr), sWater(nullptr), sClouds(nullptr), sSky(nullptr),
      projection(perspective(M_PI_4f, static_cast<float>(width) / height,
                             0.1f, terrain.chunkSize * terrain.chunks)),
      camera(vec3(0.0f, 20.0f, 0.0f)),
      cameraPos(camera.getPositionReference()),
      grid(Meshes::tessGrid(terrain.chunkSize * terrain.chunks, terrain.chunks)),
      plane(Meshes::chunk()), screen(Meshes::screen()), cubemap(Meshes::cubemap()) {

    /**** Shaders ****/
    std::string paths[4]{
        "shaders/terrain/terrain.vert",
        "shaders/terrain/terrain.tesc",
        "shaders/terrain/terrain.tese",
        "shaders/terrain/terrain.frag"
    };
    sTerrain = new Shader(paths, 4);

    paths[2] = "shaders/water/water.tese";
    paths[3] = "shaders/water/water.frag";
    sWater = new Shader(paths, 4);

    paths[0] = "shaders/clouds/clouds.vert";
    paths[1] = "shaders/clouds/clouds.frag";
    sClouds = new Shader(paths, 2);

    paths[0] = "shaders/sky/sky.vert";
    paths[1] = "shaders/sky/sky.frag";
    sSky = new Shader(paths, 2);
}

Application::~Application() {
    delete sTerrain;
    delete sWater;
    delete sClouds;
    delete sSky;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::runMinas() {
    while(!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        handleEvents();
        updateVariables();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /**** Skybox ****/
        sSky->use();
        sSky->setUniform("vpMatrix", vpMatrix);
        sSky->setUniform("cameraPos", cameraPos);
        drawSkybox();

        /**** Terrain ****/
        sTerrain->use();
        updateTerrainUniforms();

        /**** Water ****/
        sWater->use();
        updateWaterUniforms();
        drawWater();

        debugWindow();
        if(isCursorVisible) {
            terrainWindow();
            waterWindow();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void Application::runKillian() {
    while(!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        handleEvents();
        updateVariables();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /**** Skybox ****/
        sSky->use();
        sSky->setUniform("vpMatrix", vpMatrix);
        sSky->setUniform("cameraPos", cameraPos);
        drawSkybox();

        /**** Terrain ****/
        sTerrain->use();
        updateTerrainUniforms();
        sTerrain->setUniform("view", camera.getViewMatrix());
        grid.draw();

        debugWindow();
        if(isCursorVisible) {
            terrainWindow();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void Application::runRaph() {
    while(!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        handleEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        delta = glfwGetTime() - time;
        time = glfwGetTime();

        sClouds->use();
        updateCloudsUniforms();
        drawClouds();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void Application::setWindowSize(int width, int height) {
    this->width = width;
    this->height = height;

    projection[0][0] = 1.0f / (tanf(M_PI_4f / 2.0f) * static_cast<float>(width) / height);
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
                    glfwSetInputMode(window, GLFW_CURSOR, isCursorVisible ? GLFW_CURSOR_DISABLED
                                                                          : GLFW_CURSOR_NORMAL);
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
    cameraChunk.x = floor(0.5f + cameraPos.x / terrain.chunkSize);
    cameraChunk.y = floor(0.5f + cameraPos.z / terrain.chunkSize);
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

void Application::terrainWindow() {
    ImGui::Begin("Terrain Options");

    if(ImGui::CollapsingHeader("Terrain Gradient")) {
        ImGui::ColorEdit3("Color 1", &terrain.colors[0].x);
        ImGui::ColorEdit3("Color 2", &terrain.colors[1].x);
        ImGui::SliderFloat("Weight 2", &terrain.weights[1], 0.0f, terrain.weights[2]);
        ImGui::ColorEdit3("Color 3", &terrain.colors[2].x);
        ImGui::SliderFloat("Weight 3", &terrain.weights[2], terrain.weights[1], 1.0f);
        ImGui::ColorEdit3("Color 4", &terrain.colors[3].x);
    }

    if(ImGui::CollapsingHeader("Light")) {
        ImGui::InputFloat3("Light Direction", &lightDirection.x);
        ImGui::Checkbox("Fog", &terrain.isFogActive);
    }

    if(ImGui::CollapsingHeader("Noise")) {
        ImGui::InputInt("Seed", &terrain.seed);

        ImGui::Text("Amplitude Noise");
        ImGui::SliderFloat("Frequency", &terrain.freqAnoise, 0.0001f, 0.1f, "%.5f");
        ImGui::SliderFloat("Amplitude", &terrain.ampAnoise, 1.0f, 100.0f);
        ImGui::SliderInt("Octaves", &terrain.octAnoise, 1, 8);
        ImGui::InputInt("Amplitude Seed", &terrain.seedAnoise);
    }

    ImGui::End();
}

void Application::updateTerrainUniforms() {
    sTerrain->setUniform("vpMatrix", camera.getVPmatrix(projection));
    sTerrain->setUniform("cameraPos", cameraPos);
    sTerrain->setUniform("cameraChunk", cameraChunk);
    sTerrain->setUniform("chunkSize", terrain.chunkSize);

    sTerrain->setUniform("u_weights[0]", terrain.weights[0]);
    sTerrain->setUniform("u_weights[1]", terrain.weights[1]);
    sTerrain->setUniform("u_weights[2]", terrain.weights[2]);
    sTerrain->setUniform("u_weights[3]", terrain.weights[3]);
    sTerrain->setUniform("u_colors[0]", terrain.colors[0]);
    sTerrain->setUniform("u_colors[1]", terrain.colors[1]);
    sTerrain->setUniform("u_colors[2]", terrain.colors[2]);
    sTerrain->setUniform("u_colors[3]", terrain.colors[3]);

    sTerrain->setUniform("totalTerrainWidth", terrain.chunks * terrain.chunkSize / 4.0f);

    sTerrain->setUniform("lightDirection", lightDirection);

    sTerrain->setUniform("terrainSeed", terrain.seed);

    sTerrain->setUniform("freqAnoise", terrain.freqAnoise);
    sTerrain->setUniform("ampAnoise", terrain.ampAnoise);
    sTerrain->setUniform("octAnoise", static_cast<unsigned int>(terrain.octAnoise));
    sTerrain->setUniform("seedAnoise", terrain.seedAnoise);

    sTerrain->setUniform("isFogActive", terrain.isFogActive);
}

void Application::waterWindow() {
    ImGui::Begin("Water Options");
    ImGui::SliderFloat("Delta Normal", &water.deltaNormal, 0.001f, 0.1f);
    ImGui::InputFloat("Chunk Size", &water.chunkSize, 1.0f, 10.0f);
    ImGui::InputInt("Chunks", &water.chunks, 2, 10);
    ImGui::NewLine();
    ImGui::InputFloat3("Light Direction", &lightDirection.x);
    ImGui::End();
}

void Application::drawWater() {
    for(int x = 0 ; x <= water.chunks ; ++x) {
        for(int z = 0 ; z <= water.chunks ; ++z) {
            sWater->setUniform("chunk",
                               x - (water.chunks >> 1) + cameraChunk.x,
                               z - (water.chunks >> 1) + cameraChunk.y);
            plane.draw();
        }
    }
}

void Application::updateWaterUniforms() {
    sWater->setUniform("cameraPos", cameraPos);
    sWater->setUniform("vpMatrix", camera.getVPmatrix(projection));
    sWater->setUniform("deltaNormal", water.deltaNormal);
    sWater->setUniform("chunkSize", water.chunkSize);
    sWater->setUniform("lightDirection", lightDirection);
    sWater->setUniform("cameraChunk", cameraChunk);
    sWater->setUniform("time", time);
}

void Application::drawClouds() {
    screen.draw();
}

void Application::updateCloudsUniforms() {
    sClouds->setUniform("resolution", static_cast<float>(width), static_cast<float>(height));
}

void Application::drawSkybox() {
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    if(wireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

    cubemap.draw();

    if(wireframe) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}
