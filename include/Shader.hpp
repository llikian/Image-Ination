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
     * @brief Compiles then links and creates the shader program from the vertex and fragment
     * shaders located at the given paths.
     * @param vertexShaderPath The path to the vertex shader.
     * @param fragmentShaderPath The path to the fragment shader.
     */
    Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

    /**
     * @brief Deletes the shader program.
     */
    ~Shader();

    /**
     * @brief Uses the shader program.
     */
    void use();

    /**
     * @brief Finds and adds all the shader's uniforms' id's to the map.
     */
    void getUniforms();

    /**
     * @brief Sets the value of an uniform of type int.
     * @param uniform The uniform's name.
     * @param value The new value of the uniform.
     */
    template<typename... Value>
    void setUniform(const std::string& uniform, Value... value) const {
        try {
            int location = uniforms.at(uniform);
            setUniform(location, value...);
        } catch(const std::exception&) {
            std::cout << "The uniform named '" << uniform << "' is unknown.\n";
        }
    }

private:

    /**
     * @brief Sets the value of an uniform of type int.
     * @param location The uniform's location.
     * @param value The new value of the uniform.
     */
    void setUniform(int location, int value) const;

    /**
     * @brief Sets the value of an uniform of type unsigned int.
     * @param location The uniform's location.
     * @param value The new value of the uniform.
     */
    void setUniform(int location, unsigned int value) const;

    /**
     * @brief Sets the value of an uniform of type bool.
     * @param location The uniform's location.
     * @param value The new value of the uniform.
     */
    void setUniform(int location, bool value) const;

    /**
     * @brief Sets the value of an uniform of type float.
     * @param location The uniform's location.
     * @param value The new value of the uniform.
     */
    void setUniform(int location, float value) const;

    /**
     * @brief Sets the value of an uniform of type vec2.
     * @param location The uniform's location.
     * @param x The new x value of the uniform.
     * @param y The new y value of the uniform.
     */
    void setUniform(int location, float x, float y) const;

    /**
     * @brief Sets the value of an uniform of type vec3.
     * @param location The uniform's location.
     * @param x The new x value of the uniform.
     * @param y The new y value of the uniform.
     * @param z The new z value of the uniform.
     */
    void setUniform(int location, float x, float y, float z) const;

    /**
     * @brief Sets the value of an uniform of type vec4.
     * @param location The uniform's location.
     * @param x The new x value of the uniform.
     * @param y The new y value of the uniform.
     * @param z The new z value of the uniform.
     * @param w The new w value of the uniform.
     */
    void setUniform(int location, float x, float y, float z, float w) const;

    /**
     * @brief Sets the value of an uniform of type vec2.
     * @param location The uniform's location.
     * @param vec The new value of the uniform.
     */
    void setUniform(int location, const vec2& vec) const;

    /**
     * @brief Sets the value of an uniform of type vec3.
     * @param location The uniform's location.
     * @param vec The new value of the uniform.
     */
    void setUniform(int location, const vec3& vec) const;

    /**
     * @brief Sets the value of an uniform of type vec4.
     * @param location The uniform's location.
     * @param vec The new value of the uniform.
     */
    void setUniform(int location, const vec4& vec) const;

    /**
     * @brief Sets the value of an uniform of type mat4.
     * @param location The uniform's location.
     * @param matrix The new value of the uniform.
     */
    void setUniform(int location, const mat4& matrix) const;

    unsigned int id; ///< The shader program's id.
    std::unordered_map<std::string, int> uniforms; ///< Stores uniforms id's.
};