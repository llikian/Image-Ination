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
      shader(nullptr),
      projection(perspective(M_PI_4f, static_cast<float>(width) / height, 0.1f, 1000.0f)),
      camera(vec3(0.0f, 2.0f, 5.0f)) {

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
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    ImGui::GetIO().IniFilename = "lib/imgui/imgui.ini";

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

    // Sets the default diffuse and specular maps to a plain white color
    const unsigned char white[3]{255, 255, 255};
    glBindTexture(GL_TEXTURE_2D, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white);

    /**** Shaders ****/
    std::string paths[4]{"shaders/default.vert", "shaders/default.frag", "", ""};
    shader = new Shader(paths, 2);
    shader->use();
    initUniforms();

    paths[0] = "shaders/terrain.vert";
    paths[1] = "shaders/terrain.frag";
    paths[2] = "shaders/terrain.tesc";
    paths[3] = "shaders/terrain.tese";
    sTerrain = new Shader(paths, 4);
}

Application::~Application() {
    delete shader;

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

void Application::run() {
    struct Terrain {
        float size = 4.0f;
        float tesselationLevel = 64.0f;

        float deltaNormal = 0.01f;

        float frequency = 0.1f;
        float amplitude = 4.0f;
        int octave = 8;

        int chunks = 50;
    } terrain;

    struct Light {
        vec3 position{0.0f, 10.0f, 0.0f};
        vec3 direction{2.0f, 2.0f, 0.0f};

        bool isGlobal = true;
    } light;

    const mat4 IDENTITY(1.0f);

    Mesh sphere = Meshes::sphere(8, 16);
    Mesh plane = planeMesh();

    auto drawChunk = [&](int chunkX, int chunkZ) {
        sTerrain->setUniform("chunkX", chunkX);
        sTerrain->setUniform("chunkZ", chunkZ);
        plane.draw();
    };

    /**** Main Loop ****/
    while(!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        handleEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        delta = glfwGetTime() - time;
        time = glfwGetTime();

        shader->use();
        updateUniforms();
        shader->setUniform("lightPos", light.position);

        if(!light.isGlobal) {
            calculateMVP(translate(scale(IDENTITY, vec3(0.25f)), light.position));
            shader->setUniform("isLight", true);
            sphere.draw();
            shader->setUniform("isLight", false);
        }

        sTerrain->use();
        sTerrain->setUniform("cameraPos", camera.getPosition());
        sTerrain->setUniform("vpMatrix", camera.getVPmatrix(projection));
        sTerrain->setUniform("deltaNormal", terrain.deltaNormal);
        sTerrain->setUniform("frequency", terrain.frequency);
        sTerrain->setUniform("amplitude", terrain.amplitude);
        sTerrain->setUniform("octave", terrain.octave);
        sTerrain->setUniform("terrainSize", terrain.size);
        sTerrain->setUniform("tesselationLevel", terrain.tesselationLevel);
        sTerrain->setUniform("light.position", light.position);
        sTerrain->setUniform("light.direction", light.direction);
        sTerrain->setUniform("light.isGlobal", light.isGlobal);

        for(int x = 0 ; x <= terrain.chunks ; ++x) {
            for(int z = 0 ; z <= terrain.chunks ; ++z) {
                drawChunk(x - (terrain.chunks >> 1), z - (terrain.chunks >> 1));
            }
        }

        if(isCursorVisible) {
            ImGui::Begin("Test");
            ImGui::InputFloat("Frequency", &terrain.frequency, 0.1f, 1.0f);
            ImGui::InputFloat("Amplitude", &terrain.amplitude, 0.1f, 1.0f);
            ImGui::InputInt("Octaves", &terrain.octave, 1, 8);
            ImGui::NewLine();
            ImGui::InputFloat("Delta Normal", &terrain.deltaNormal, 0.001f, 0.01f);
            ImGui::NewLine();
            ImGui::InputFloat("Terrain Size", &terrain.size, 1.0f, 10.0f);
            ImGui::InputFloat("Tesselation Level", &terrain.tesselationLevel, 2.0f, 8.0f);
            ImGui::NewLine();
            ImGui::InputInt("Chunks", &terrain.chunks, 2, 10);
            ImGui::NewLine();
            ImGui::Checkbox("Global Lighting", &light.isGlobal);
            if(light.isGlobal) {
                ImGui::InputFloat3("Light Direction", &light.direction.x);
            } else {
                ImGui::InputFloat3("Light Position", &light.position.x);
            }
            ImGui::End();
        }

        ImGui::Begin("Debug");
        ImGui::Text("%d FPS | %.2fms/frame"
                    "", static_cast<int>(1.0f / delta), 1000.0f * delta);
        ImGui::End();

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

void Application::initUniforms() {
    calculateMVP(mat4(1.0f));
}

void Application::updateUniforms() {
    shader->setUniform("cameraPos", camera.getPosition());
}

void Application::calculateMVP(const mat4& model) {
    shader->setUniform("mvp", camera.getVPmatrix(projection) * model);
    shader->setUniform("model", model);
}
