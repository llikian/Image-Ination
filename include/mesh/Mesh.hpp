/***************************************************************************************************
 * @file  Mesh.hpp
 * @brief Declaration of the Mesh class
 **************************************************************************************************/

#pragma once

#include <vector>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "Shader.hpp"

using namespace glm;

/**
 * @class Mesh
 * @brief Represents a 3D mesh that can be created and rendererd.
 */
class Mesh {
public:
    /**
     * @brief Constructs a Mesh with a certain primitive and generates a VAO, a VBO and an EBO.
     * @param primitive The primitive. Can be one of:\n
     *   GL_POINTS\n
     *   GL_LINES\n
     *   GL_TRIANGLES\n
     *   etc…
     */
    Mesh(unsigned int primitive);

    /**
     * @brief Constructs a Mesh with the same data as another.
     * @param mesh The mesh to copy.
     */
    Mesh(const Mesh& mesh);

    /**
     * @brief Copies a mesh's data in this one.
     * @param mesh The mesh to copy.
     * @return A reference to this mesh.
     */
    Mesh& operator= (const Mesh& mesh);

    /**
     * @brief Deletes the VAO, the VBO and the EBO.
     */
    ~Mesh();

    /**
     * @brief Renders the mesh.
     */
    void draw();

    /**
     * @brief Adds a position to the data.
     * @param x, y, z The point's coordinates.
     */
    void addPosition(float x, float y, float z);

    /**
     * @brief Adds a position to the data.
     * @param position The point's coordinates.
     */
    void addPosition(const vec3& position);

    /**
     * @brief Adds a normal to the data.
     * @param x, y, z The vector's values.
     */
    void addNormal(float x, float y, float z);

    /**
     * @brief Adds a normal to the data.
     * @param normal The vector.
     */
    void addNormal(const vec3& normal);

    /**
     * @brief Adds texture coordinates to the data.
     * @param x, y The texture coordinates.
     */
    void addTexCoord(float x, float y);

    /**
     * @brief Adds texture coordinates to the data.
     * @param texCoord The texture coordinates.
     */
    void addTexCoord(const vec2& texCoord);

    /**
     * @brief Adds a color to the data.
     * @param r, g, b The color's values.
     */
    void addColor(float r, float g, float b);

    /**
     * @brief Adds a color to the data.
     * @param color The color.
     */
    void addColor(const vec3& color);

    /**
     * @brief Adds an index to the indices.
     * @param index The index.
     */
    void addIndex(unsigned int index);

    /**
     * @brief Adds a triangle to the indices.
     * @param top, left, right The triangle's indices.
     */
    void addTriangle(unsigned int top, unsigned int left, unsigned int right);

    /**
     * @brief Adds a face to the indices.
     * @param topL, bottomL, bottomR, topR The face's indices.
     */
    void addFace(unsigned int topL, unsigned int bottomL, unsigned int bottomR, unsigned int topR);

    /**
     * @brief Getter for the primitive member.
     * @return The primitive of the mesh.
     */
    unsigned int getPrimitive() const;

    /**
     * @brief Getter for the attributes member.
     * @return The attributes of the mesh.
     */
    u_int8_t getAttributes() const;

    /**
     * @brief Getter fot the data member.
     * @return The data of the mesh.
     */
    const std::vector<float>* getData() const;

    /**
     * @brief Getter for the indices member.
     * @return The indices of the mesh.
     */
    const std::vector<unsigned int>* getIndices() const;

private:
    /**
     * @brief Binds the data to the VBO correctly. If indices were sepcified also binds the
     * corresponding data the EBO. Binds the VBO (and the EBO if available) to the VAO.
     */
    void bindBuffers();

    /**
     * @brief Calculates the stride according to which attributes are enabled.
     * @return The stride between a vertex attribute's value and the next.
     */
    unsigned int getStride() const;

    unsigned int primitive; ///< 3D Primitive used to draw. e.g. GL_TRIANGLES, GL_LINES, etc…

    bool shouldBind; ///< Whether the buffer should be bound before drawing.

    unsigned int VAO; ///< Vertex Array Object
    unsigned int VBO; ///< Vertex Buffer Object
    unsigned int EBO; ///< Element Buffer Object

    /**
     * Bit masks for which attributes are enabled. For now the attributes are from right to
     * left (in little endian) :\n
     *   Position (vec3) : Is always enabled.\n
     *   Normal (vec3)\n
     *   Texture Coordinates (vec2)\n
     *   Color (vec3)
     */
    u_int8_t attributes;

    /**
     * Attributes data. The currently available attributes are:\n
     *   0 - Position\n
     *   1 - Normal\n
     *   2 - Texture Coordinates\n
     *   3 - Color
     */
    std::vector<float> data;

    /**
     * Vector that can contain vertex indices in order to draw them according to the active
     * primitive. So for example if the primitive is GL_TRIANGLES, then the vertices corresponding
     * to 3 consecutive indices will form a triangle.\n
     * Filling this vector up is optional, if it is empty, the consecutive vertices in the data
     * will be drawn according to the primitive.
     */
    std::vector<unsigned int> indices;
};