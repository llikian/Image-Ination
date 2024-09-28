/***************************************************************************************************
 * @file  Application.cpp
 * @brief Implementation of the Application class
 **************************************************************************************************/

#include "Application.hpp"

#include "mesh/meshes.hpp"
#include "callbacks.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "misc/cpp/imgui_stdlib.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

Application::Application()
    : window(nullptr), width(1600), height(900),
      time(0.0f), delta(0.0f),
      wireframe(false), cullface(true), isCursorVisible(false),
      sTerrain(nullptr),
      projection(perspective(M_PI_4f, static_cast<float>(width) / height, 0.1f, 1000.0f)),
      camera(vec3(0.0f, 20.0f, 0.0f)) {

    /**** GLFW ****/
    if(!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Image-Ination", nullptr, nullptr);
    if(!window) {
        throw std::runtime_error("Failed to create window.");
    }

    glfwMakeContextCurrent(window);
    glfwMaximizeWindow(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(0);

    mousePos.x = width / 2.0f;
    mousePos.y = height / 2.0f;

    /**** GLFW Callbacks ****/
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
//    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
//    glfwSetScrollCallback(window, scrollCallback);

    /**** ImGui ****/
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().IniFilename = "lib/imgui/imgui.ini";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    /**** GLAD ****/
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD.");
    }

    /**** OpenGL ****/
    glViewport(0, 0, width, height);
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

    /**** Shaders ****/
    std::string paths[4]{
        "shaders/terrain.vert",
        "shaders/terrain.frag",
        "shaders/terrain.tesc",
        "shaders/terrain.tese"
    };
    sTerrain = new Shader(paths, 4);

    paths[1] = "shaders/water.frag";
    paths[3] = "shaders/water.tese";
    sWater = new Shader(paths, 4);
}

Application::~Application() {
    delete sTerrain;
    delete sWater;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

Mesh planeMesh() {
    Mesh mesh(GL_PATCHES);

    mesh.addPosition(-0.5f, 0.0f, 0.5f);
    mesh.addPosition(0.5f, 0.0f, 0.5f);
    mesh.addPosition(0.5f, 0.0f, -0.5f);
    mesh.addPosition(-0.5f, 0.0f, -0.5f);

    return mesh;
}

void Application::runMinas() {
    struct Water {
        float deltaNormal = 0.01f;
        float chunkSize = 4.0f;
        int chunks = 10;
    } water;

    struct Terrain {
        float deltaNormal = 0.01f;
        float chunkSize = 4.0f;
        int chunks = 10;
    } terrain;

    const mat4 IDENTITY(1.0f);
    const vec3 skyColor(0.306f, 0.706f, 0.89f);
    vec3 lightDirection(2.0f, 2.0f, 0.0f);
    vec3 cameraPos;

    Mesh plane = planeMesh();

    auto drawChunk = [&](Shader* shader, int chunkX, int chunkZ) {
        shader->setUniform("chunk", chunkX, chunkZ);
        plane.draw();
    };

    /**** Main Loop ****/
    while(!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        handleEvents();

        glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        delta = glfwGetTime() - time;
        time = glfwGetTime();
        cameraPos = camera.getPosition();

        ImGui::Begin("Debug");
        ImGui::Text("%d FPS | %.2fms/frame", static_cast<int>(1.0f / delta), 1000.0f * delta);
        ImGui::Text("Position: (%.2f ; %.2f ; %.2f)", cameraPos.x, cameraPos.y, cameraPos.z);
        ImGui::End();

        sTerrain->use();
        sTerrain->setUniform("cameraPos", cameraPos);
        sTerrain->setUniform("vpMatrix", camera.getVPmatrix(projection));
        sTerrain->setUniform("deltaNormal", terrain.deltaNormal);
        sTerrain->setUniform("chunkSize", terrain.chunkSize);
        sTerrain->setUniform("lightDirection", lightDirection);
        sTerrain->setUniform("cameraChunk",
                             static_cast<int>(cameraPos.x / terrain.chunkSize + 0.5f),
                             static_cast<int>(cameraPos.z / terrain.chunkSize + 0.5f));

        for(int x = 0 ; x <= terrain.chunks ; ++x) {
            for(int z = 0 ; z <= terrain.chunks ; ++z) {
                drawChunk(sTerrain, x - (terrain.chunks >> 1), z - (terrain.chunks >> 1));
            }
        }

        sWater->use();
        sWater->setUniform("cameraPos", cameraPos);
        sWater->setUniform("vpMatrix", camera.getVPmatrix(projection));
        sWater->setUniform("deltaNormal", water.deltaNormal);
        sWater->setUniform("chunkSize", water.chunkSize);
        sWater->setUniform("lightDirection", lightDirection);
        sWater->setUniform("cameraChunk",
                           static_cast<int>(cameraPos.x / terrain.chunkSize + 0.5f),
                           static_cast<int>(cameraPos.z / terrain.chunkSize + 0.5f));

        sWater->setUniform("time", time);

        for(int x = 0 ; x <= water.chunks ; ++x) {
            for(int z = 0 ; z <= water.chunks ; ++z) {
                drawChunk(sWater, x - (water.chunks >> 1), z - (water.chunks >> 1));
            }
        }

        if(isCursorVisible) {
            ImGui::Begin("Water Options");
            ImGui::SliderFloat("Delta Normal", &water.deltaNormal, 0.001f, 0.1f);
            ImGui::InputFloat("Chunk Size", &water.chunkSize, 1.0f, 10.0f);
            ImGui::InputInt("Chunks", &water.chunks, 2, 10);
            ImGui::NewLine();
            ImGui::InputFloat3("Light Direction", &lightDirection.x);
            ImGui::End();
        }

        if(isCursorVisible) {
            ImGui::Begin("Terrain Options");
            ImGui::SliderFloat("Delta Normal", &terrain.deltaNormal, 0.001f, 0.1f);
            ImGui::InputFloat("Chunk Size", &terrain.chunkSize, 1.0f, 10.0f);
            ImGui::InputInt("Chunks", &terrain.chunks, 2, 10);
            ImGui::NewLine();
            ImGui::InputFloat3("Light Direction", &lightDirection.x);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void Application::runKillian() {
    struct Terrain {
        float chunkSize = 8.0f;
        int chunks = 200;
        float tesselationFactor = 8.0f;

        float weights[4] = {0.0f, 0.2f, 0.5f, 1.0f};
        vec3 colors[4]{
            vec3(0.184f, 0.694f, 0.831f),
            vec3(0.357f, 0.6f, 0.369f),
            vec3(0.58f, 0.49f, 0.388f),
            vec3(0.969f, 1.0f, 0.996f)
        };
    } terrain;

    const mat4 IDENTITY(1.0f);
    const vec3 skyColor(0.306f, 0.706f, 0.89f);
    vec3 lightDirection(2.0f, 2.0f, 0.0f);

    vec3 cameraPos;
    vec2 cameraChunk;

    Mesh plane = planeMesh();

    auto drawChunk = [&](int chunkX, int chunkZ) {
        sTerrain->setUniform("chunk", chunkX + cameraChunk.x, chunkZ + cameraChunk.y);
        plane.draw();
    };

    auto drawTerrain = [&]() {
        for(int x = 0 ; x <= terrain.chunks ; ++x) {
            for(int z = 0 ; z <= terrain.chunks ; ++z) {
                drawChunk(x - (terrain.chunks >> 1), z - (terrain.chunks >> 1));
            }
        }
    };

    bool autoMove = false;

    /**** Main Loop ****/
    while(!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        handleEvents();

        glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        delta = glfwGetTime() - time;
        time = glfwGetTime();
        cameraPos = camera.getPosition();
        cameraChunk.x = floor(cameraPos.x / terrain.chunkSize + 0.5f);
        cameraChunk.y = floor(cameraPos.z / terrain.chunkSize + 0.5f);

        ImGui::Begin("Debug");
        ImGui::Text("%d FPS | %.2fms/frame", static_cast<int>(1.0f / delta), 1000.0f * delta);
        ImGui::Text("Position: (%.2f ; %.2f ; %.2f)", cameraPos.x, cameraPos.y, cameraPos.z);
        ImGui::End();

        sTerrain->use();
        sTerrain->setUniform("vpMatrix", camera.getVPmatrix(projection));
        sTerrain->setUniform("cameraPos", cameraPos);
        sTerrain->setUniform("cameraChunk", cameraChunk);
        sTerrain->setUniform("chunkSize", terrain.chunkSize);
        sTerrain->setUniform("tesselationFactor", terrain.tesselationFactor);

        sTerrain->setUniform("u_weights[0]", terrain.weights[0]);
        sTerrain->setUniform("u_weights[1]", terrain.weights[1]);
        sTerrain->setUniform("u_weights[2]", terrain.weights[2]);
        sTerrain->setUniform("u_weights[3]", terrain.weights[3]);
        sTerrain->setUniform("u_colors[0]", terrain.colors[0]);
        sTerrain->setUniform("u_colors[1]", terrain.colors[1]);
        sTerrain->setUniform("u_colors[2]", terrain.colors[2]);
        sTerrain->setUniform("u_colors[3]", terrain.colors[3]);

        sTerrain->setUniform("skyColor", skyColor);
        sTerrain->setUniform("totalTerrainWidth", terrain.chunks * terrain.chunkSize / 4.0f);

        sTerrain->setUniform("lightDirection", lightDirection);

        drawTerrain();

        if(isCursorVisible) {
            ImGui::Begin("Terrain Options");
            ImGui::InputFloat("Chunk Size", &terrain.chunkSize, 1.0f, 10.0f);
            ImGui::InputInt("Chunks", &terrain.chunks, 2, 10);
            ImGui::SliderFloat("Tesselation Factor", &terrain.tesselationFactor, 1.0f, 64.0f);
            ImGui::NewLine();
            ImGui::ColorEdit3("Color 1", &terrain.colors[0].x);
            ImGui::ColorEdit3("Color 2", &terrain.colors[1].x);
            ImGui::SliderFloat("Weight 2", &terrain.weights[1], 0.0f, terrain.weights[2]);
            ImGui::ColorEdit3("Color 3", &terrain.colors[2].x);
            ImGui::SliderFloat("Weight 3", &terrain.weights[2], terrain.weights[1], 1.0f);
            ImGui::ColorEdit3("Color 4", &terrain.colors[3].x);
            ImGui::NewLine();
            ImGui::InputFloat3("Light Direction", &lightDirection.x);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
}

void Application::runRaph() {
    const mat4 IDENTITY(1.0f);
    const vec3 skyColor(0.306f, 0.706f, 0.89f);

    /**** Main Loop ****/
    while(!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        handleEvents();

        glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        delta = glfwGetTime() - time;
        time = glfwGetTime();

        if(isCursorVisible) {
            ImGui::Begin("Options");

            ImGui::End();
        }

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
                    glfwSetInputMode(window, GLFW_CURSOR,
                                     isCursorVisible ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
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
