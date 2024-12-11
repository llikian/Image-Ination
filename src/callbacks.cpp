/***************************************************************************************************
 * @file  callbacks.cpp
 * @brief Implementation of the callback functions for OpenGL
 **************************************************************************************************/

#include "callbacks.hpp"

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    static_cast<Application*>(glfwGetWindowUserPointer(window))->setWindowSize(width, height);
}

void frameBufferSizeCallback(GLFWwindow* /* window */, int width, int height) {
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int /* scancode */, int action, int mods) {
    static_cast<Application*>(glfwGetWindowUserPointer(window))->handleKeyCallback(key, action, mods);
}

void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos) {
    static_cast<Application*>(glfwGetWindowUserPointer(window))->handleCursorPositionEvent(xPos, yPos);
}
