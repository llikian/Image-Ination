/***************************************************************************************************
 * @file  meshes.hpp
 * @brief Declaration of functions to create meshes
 **************************************************************************************************/

#pragma once

#include "Mesh.hpp"

namespace Meshes {
    /**
     * @brief Creates the mesh for a cube with side of size 1 where all the faces will have the
     * same texture.
     */
    Mesh cube();

    /**
     * @brief Creates the mesh for a cube with sides of size 1 that can have different textures
     * for each face.
     */
    Mesh texturedCube();

    /**
     * @brief Creates the mesh for a wireframe cube. The cube will have a side of size 1.
     */
    Mesh wireframeCube();

    /**
     * @brief Creates the mesh for a cube without normals.
     */
    Mesh plainCube();

    /**
     * @brief Creates the mesh for a grid on the XZ plane.
     * @param size Size of one line of the grid.
     * @param divisions Number of divisions on the grid.
     */
    Mesh grid(float size, int divisions);

    /**
     * @brief Creates the mesh for the axes. The x axis is red, the y axis is green and the z axes
     * is blue.
     * @param size The size of the line for each axes.
     */
    Mesh axes(float size);

    /**
     * @brief Creates the mesh for a sphere with a radius of 1.
     * @param divTheta
     * @param divPhi
     */
    Mesh sphere(int divTheta, int divPhi);

    /**
     * @brief Creates the mesh for a sphere with a radius of 1.
     * @param divTheta
     * @param divPhi
     */
    Mesh texturedSphere(int divTheta, int divPhi);

    /**
     * @brief Creates the mesh for a plane with a certain size. The plane will be a cut of the XZ
     * plane and will be centered at the origin.
     * @param size The size of the plane, its dimensions will be (size ; 0 ; size).
     */
    Mesh plane(float size);

    /**
     * @brief Creates the mesh for a plane of a certain size with normals. The plane will be a cut
     * of the XZ plane and will be centered at the origin.
     * @param size The size of the plane, its dimensions will be (size ; 0 ; size).
     */
    Mesh nplane(float size);

    /**
     * @brief Creates the mesh for the screen. It's just a plane from (1 ; 1 ; 0) to (-1 ; -1 ; 0)
     * that can be used to render things directly on the screen.
     */
    Mesh screen();
}