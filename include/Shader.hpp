/***************************************************************************************************
 * @file  Shader.hpp
 * @brief Declaration of the Shader class
 **************************************************************************************************/

#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

using namespace glm;

/**
 * @class Shader
 * @brief Compiles, links and creates a shader program that can be then be bound. Can set the value
 * of uniforms in the shader program.
 */
class Shader {
public:

    /**
     * @brief Creates a shader program and compiles then attaches the shaders at the specified paths
     * to it.
     * @param paths The paths to each of the different shaders.
     * @param count The amount of shaders to attach.
     */
    Shader(const std::string* paths, unsigned int count, const std::string& name);

    /**
     * @brief Deletes the shader program.
     */
    ~Shader();

    /**
     * @brief Compiles a shader and returns its corresponding id.
     * @param path The path to the shader file.
     * @return The shader's corresponding id.
     */
    unsigned int compileShader(const std::string& path);

    /**
     * @brief Uses the shader program.
     */
    void use();

    /**
     * @brief Finds and adds all the shader's uniforms' id's to the map.
     */
    void getUniforms();

    /**
     * @brief Sets the value of a uniform of any of the available types.
     * @param uniform The uniform's name.
     * @param value The new value of the uniform.
     */
    template<typename... Value>
    void setUniform(const std::string& uniform, Value... value) {
        try {
            setUniform(uniforms.at(uniform), value...);
        } catch(const std::exception&) {
            if(!unknownUniforms.contains(uniform)) {
                std::cout << "The uniform named '" << uniform << "' is unknown or unused";
                std::cout << " in shader program '" << name << "'.\n";

                unknownUniforms.emplace(uniform, true);
            }
        }
    }

private:

    /**
     * @brief Sets the value of a uniform of type int.
     * @param location The uniform's location.
     * @param value The new value of the uniform.
     */
    void setUniform(int location, int value) const;

    /**
     * @brief Sets the value of a uniform of type ivec2.
     * @param location The uniform's location.
     * @param x The new x value of the uniform.
     * @param y The new y value of the uniform.
     */
    void setUniform(int location, int x, int y) const;

    /**
     * @brief Sets the value of a uniform of type ivec3.
     * @param location The uniform's location.
     * @param x The new x value of the uniform.
     * @param y The new y value of the uniform.
     * @param z The new z value of the uniform.
     */
    void setUniform(int location, int x, int y, int z) const;

    /**
     * @brief Sets the value of a uniform of type ivec4.
     * @param location The uniform's location.
     * @param x The new x value of the uniform.
     * @param y The new y value of the uniform.
     * @param z The new z value of the uniform.
     * @param w The new w value of the uniform.
     */
    void setUniform(int location, int x, int y, int z, int w) const;

    /**
     * @brief Sets the value of a uniform of type unsigned int.
     * @param location The uniform's location.
     * @param value The new value of the uniform.
     */
    void setUniform(int location, unsigned int value) const;

    /**
     * @brief Sets the value of a uniform of type bool.
     * @param location The uniform's location.
     * @param value The new value of the uniform.
     */
    void setUniform(int location, bool value) const;

    /**
     * @brief Sets the value of a uniform of type float.
     * @param location The uniform's location.
     * @param value The new value of the uniform.
     */
    void setUniform(int location, float value) const;

    /**
     * @brief Sets the value of a uniform of type vec2.
     * @param location The uniform's location.
     * @param x The new x value of the uniform.
     * @param y The new y value of the uniform.
     */
    void setUniform(int location, float x, float y) const;

    /**
     * @brief Sets the value of a uniform of type vec3.
     * @param location The uniform's location.
     * @param x The new x value of the uniform.
     * @param y The new y value of the uniform.
     * @param z The new z value of the uniform.
     */
    void setUniform(int location, float x, float y, float z) const;

    /**
     * @brief Sets the value of a uniform of type vec4.
     * @param location The uniform's location.
     * @param x The new x value of the uniform.
     * @param y The new y value of the uniform.
     * @param z The new z value of the uniform.
     * @param w The new w value of the uniform.
     */
    void setUniform(int location, float x, float y, float z, float w) const;

    /**
     * @brief Sets the value of a uniform of type vec2.
     * @param location The uniform's location.
     * @param vec The new value of the uniform.
     */
    void setUniform(int location, const vec2& vec) const;

    /**
     * @brief Sets the value of a uniform of type vec3.
     * @param location The uniform's location.
     * @param vec The new value of the uniform.
     */
    void setUniform(int location, const vec3& vec) const;

    /**
     * @brief Sets the value of a uniform of type vec4.
     * @param location The uniform's location.
     * @param vec The new value of the uniform.
     */
    void setUniform(int location, const vec4& vec) const;

    /**
     * @brief Sets the value of a uniform of type mat4.
     * @param location The uniform's location.
     * @param matrix The new value of the uniform.
     */
    void setUniform(int location, const mat4& matrix) const;

    unsigned int id; ///< The shader program's id.
    std::string name; ///< The shader's name.
    std::unordered_map<std::string, int> uniforms; ///< Stores uniforms id's.
    std::unordered_map<std::string, bool> unknownUniforms; ///< Stores unknown uniforms.
};