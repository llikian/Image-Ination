/***************************************************************************************************
 * @file  Shader.cpp
 * @brief Implementation of the Shader class
 **************************************************************************************************/

#include "Shader.hpp"

#include <glad/glad.h>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    int messageLength;

    /**** Vertex Shader ****/
    std::ifstream vertexShaderFile(vertexShaderPath);
    if(!vertexShaderFile.is_open()) {
        throw std::runtime_error("Failed to open vertex shader.");
    }

    std::string vertexShaderCode = (std::stringstream() << vertexShaderFile.rdbuf()).str();
    const char* vertexShader = vertexShaderCode.c_str();
    unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShader, nullptr);
    glCompileShader(vertexShaderID);

    glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &messageLength);
    if(messageLength > 0) {
        char* message = new char[messageLength];
        glGetShaderInfoLog(vertexShaderID, messageLength, nullptr, message);

        std::string errorMessage = "Failed to compile vertex shader :\n";
        errorMessage += message;

        delete[] message;

        throw std::runtime_error(errorMessage);
    }

    /**** Fragment Shader ****/
    std::ifstream fragmentShaderFile(fragmentShaderPath);
    if(!fragmentShaderFile.is_open()) {
        throw std::runtime_error("Failed to open fragment shader.");
    }

    std::string fragmentShaderCode = (std::stringstream() << fragmentShaderFile.rdbuf()).str();
    const char* fragmentShader = fragmentShaderCode.c_str();
    unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragmentShader, nullptr);
    glCompileShader(fragmentShaderID);

    glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &messageLength);
    if(messageLength > 0) {
        char* message = new char[messageLength];
        glGetShaderInfoLog(fragmentShaderID, messageLength, nullptr, message);

        std::string errorMessage = "Failed to compile fragment shader :\n";
        errorMessage += message;

        delete[] message;

        throw std::runtime_error(errorMessage);
    }

    /**** Shader Program ****/
    id = glCreateProgram();
    glAttachShader(id, vertexShaderID);
    glAttachShader(id, fragmentShaderID);
    glLinkProgram(id);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &messageLength);
    if(messageLength > 0) {
        char* message = new char[messageLength];
        glGetProgramInfoLog(id, messageLength, nullptr, message);

        std::string errorMessage = "Failed to link shader program :\n";
        errorMessage += message;

        delete[] message;

        throw std::runtime_error(errorMessage);
    }

    getUniforms();
}

Shader::~Shader() {
    glDeleteProgram(id);
}

void Shader::use() {
    glUseProgram(id);
}

void Shader::getUniforms() {
    int MAX_CHAR;
    glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &MAX_CHAR);

    int length;
    unsigned int type;
    int size;
    char* name = new char[MAX_CHAR];

    int count;
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);

    for(unsigned int i = 0u ; i < static_cast<unsigned int>(count) ; ++i) {
        glGetActiveUniform(id, i, MAX_CHAR, &length, &size, &type, name);
        uniforms.emplace(name, i);
    }

    delete[] name;
}

void Shader::setUniform(int location, int value) const {
    glUniform1i(location, value);
}

void Shader::setUniform(int location, unsigned int value) const {
    glUniform1ui(location, value);
}

void Shader::setUniform(int location, bool value) const {
    glUniform1i(location, static_cast<int>(value));
}

void Shader::setUniform(int location, float value) const {
    glUniform1f(location, value);
}

void Shader::setUniform(int location, float x, float y) const {
    glUniform2f(location, x, y);
}

void Shader::setUniform(int location, float x, float y, float z) const {
    glUniform3f(location, x, y, z);
}

void Shader::setUniform(int location, float x, float y, float z, float w) const {
    glUniform4f(location, x, y, z, w);
}

void Shader::setUniform(int location, const vec2& vec) const {
    glUniform2fv(location, 1, &vec.x);
}

void Shader::setUniform(int location, const vec3& vec) const {
    glUniform3fv(location, 1, &vec.x);
}

void Shader::setUniform(int location, const vec4& vec) const {
    glUniform4fv(location, 1, &vec.x);
}

void Shader::setUniform(int location, const mat4& matrix) const {
    glUniformMatrix4fv(location, 1, false, &(matrix[0][0]));
}
