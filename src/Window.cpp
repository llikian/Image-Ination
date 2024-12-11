/***************************************************************************************************
 * @file  cpp
 * @brief Implementation of the Window class
 **************************************************************************************************/

#include "Window.hpp"

#include <stdexcept>

#include "callbacks.hpp"

Window::Window(void* userPointer) : window(nullptr), width(1600), height(800) {
    /**** GLFW ****/
    if(!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Image-Ination", nullptr, nullptr);
    if(!window) {
        throw std::runtime_error("Failed to create ");
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, userPointer);
    glfwMaximizeWindow(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(0); // Disables V-Sync

    /**** Width & Height ****/
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    width = w;
    height = h;

    /**** GLFW Callbacks ****/
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

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
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

Window::operator GLFWwindow*() {
    return window;
}

void Window::updateSize(unsigned int width, unsigned int height) {
    this->width = width;
    this->height = height;
}

unsigned int Window::getWidth() const {
    return width;
}

unsigned int Window::getHeight() const {
    return height;
}

vec2 Window::getResolution() const {
    return vec2(static_cast<float>(width), static_cast<float>(height));
}

float Window::getRatio() const {
    return static_cast<float>(width) / static_cast<float>(height);
}
