/***************************************************************************************************
 * @file  Shader.cpp
 * @brief Implementation of the Shader class
 **************************************************************************************************/

#include "Shader.hpp"

#include <glad/glad.h>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string* paths, unsigned int count, const std::string& name = "") :
    id(glCreateProgram()),
    name(name) {

    /**** Shader Name ****/
    if(name.size() == 0) {
        this->name = "shader" + std::to_string(id);
    }

    /**** Shaders ****/
    unsigned int shaderID;
    for(unsigned int i = 0 ; i < count ; ++i) {
        shaderID = compileShader(paths[i]);
        glAttachShader(id, shaderID);
        glDeleteShader(shaderID);
    }

    /**** Shader Program ****/
    glLinkProgram(id);

    int messageLength;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &messageLength);
    if(messageLength > 0) {
        char* message = new char[messageLength];
        glGetProgramInfoLog(id, messageLength, nullptr, message);

        std::string errorMessage = "Failed to link shader program '";
        errorMessage += name;
        errorMessage += "':\n";
        errorMessage += message;

        delete[] message;

        throw std::runtime_error(errorMessage);
    }

    getUniforms();
}

Shader::~Shader() {
    glDeleteProgram(id);
}

unsigned int Shader::compileShader(const std::string& path) {
    std::string extension = path.substr(path.find_last_of('.') + 1);

    std::string shaderTypeName;
    unsigned int shaderType;
    switch(extension[0]) {
        case 'v':
            shaderTypeName = "vertex";
            shaderType = GL_VERTEX_SHADER;
            break;
        case 'f':
            shaderTypeName = "fragment";
            shaderType = GL_FRAGMENT_SHADER;
            break;
        case 't':
            if(extension[3] == 'c') {
                shaderTypeName = "tesselation control";
                shaderType = GL_TESS_CONTROL_SHADER;
            } else {
                shaderTypeName = "tesselation evaluation";
                shaderType = GL_TESS_EVALUATION_SHADER;
            }
            break;
        case 'c':
            shaderTypeName = "compute";
            shaderType = GL_COMPUTE_SHADER;
            break;
        case 'g':
            shaderTypeName = "geometry";
            shaderType = GL_GEOMETRY_SHADER;
            break;
    }

    std::ifstream file(path);
    if(!file.is_open()) {
        throw std::runtime_error(std::string("Failed to open " + shaderTypeName + " shader."));
    }

    std::string rawCode = (std::stringstream() << file.rdbuf()).str();
    const char* code = rawCode.c_str();
    unsigned int id = glCreateShader(shaderType);
    glShaderSource(id, 1, &code, nullptr);
    glCompileShader(id);

    int messageLength;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &messageLength);
    if(messageLength > 0) {
        char* message = new char[messageLength];
        glGetShaderInfoLog(id, messageLength, nullptr, message);

        std::string errorMessage = "Failed to compile " + shaderTypeName + " shader for shader program '";
        errorMessage += name;
        errorMessage += "':\n";
        errorMessage += message;

        delete[] message;

        throw std::runtime_error(errorMessage);
    }

    return id;
}

void Shader::use() {
    glUseProgram(id);
}

void Shader::getUniforms() {
    use();

    int MAX_CHAR;
    glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &MAX_CHAR);

    int length;
    unsigned int type;
    int size;
    char* name = new char[MAX_CHAR];

    unsigned int count;
    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, reinterpret_cast<int*>(&count));

    for(unsigned int i = 0u ; i < count ; ++i) {
        glGetActiveUniform(id, i, MAX_CHAR, &length, &size, &type, name);

        if(size == 1) {
            uniforms.emplace(name, glGetUniformLocation(id, name));
        } else {
            std::string nameIndex;
            for(unsigned int j = 0 ; j < static_cast<unsigned int>(size) ; ++j) {
                name[length - 2] = '\0';
                nameIndex = name;
                nameIndex += std::to_string(j) + ']';
                uniforms.emplace(nameIndex, glGetUniformLocation(id, nameIndex.c_str()));
            }
        }
    }

    delete[] name;
}

void Shader::setUniform(int location, int value) const {
    glUniform1i(location, value);
}

void Shader::setUniform(int location, int x, int y) const {
    glUniform2i(location, x, y);
}

void Shader::setUniform(int location, int x, int y, int z) const {
    glUniform3i(location, x, y, z);
}

void Shader::setUniform(int location, int x, int y, int z, int w) const {
    glUniform4i(location, x, y, z, w);
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
