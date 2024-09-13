/***************************************************************************************************
 * @file  Application.cpp
 * @brief Implementation of the Application class
 **************************************************************************************************/

#include "Application.hpp"

#include "mesh/meshes.hpp"
#include "callbacks.hpp"
#include <glm/gtc/matrix_transform.hpp>

Application::Application()
    : window(nullptr), width(1600), height(900),
      time(0.0f), delta(0.0f),
      cursorVisible(false),
      shader(nullptr),
      projection(perspective(M_PI_4f, static_cast<float>(width) / height, 0.1f, 100.0f)),
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

    mousePos.x = width / 2.0f;
    mousePos.y = height / 2.0f;

    /**** GLFW Callbacks ****/
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
//    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
//    glfwSetScrollCallback(window, scrollCallback);

    /**** GLAD ****/
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD.");
    }

    /**** OpenGL ****/
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glActiveTexture(GL_TEXTURE0);

    // Sets the default diffuse and specular maps to a plain white color
    const unsigned char white[3]{255, 255, 255};
    glBindTexture(GL_TEXTURE_2D, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white);

    /**** Shaders ****/
    shader = new Shader("shaders/default.vert", "shaders/default.frag");
    shader->use();
    initUniforms();
}

Application::~Application() {
    delete shader;

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::run() {
    Mesh sphere = Meshes::sphere(32, 16);

    /**** Main Loop ****/
    while(!glfwWindowShouldClose(window)) {
        handleEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        delta = glfwGetTime() - time;
        time = glfwGetTime();

        shader->use();
        updateUniforms();

        calculateMVP(mat4(1.0f));
        sphere.draw();

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
    if(!cursorVisible) {
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
                                     cursorVisible ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
                    cursorVisible = !cursorVisible;

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
    shader->setUniform("mvp", std::move(camera.getVPmatrix(projection) * model));
    shader->setUniform("model", model);
}
