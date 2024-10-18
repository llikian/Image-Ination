/***************************************************************************************************
 * @file  meshes.cpp
 * @brief Implementation of functions to create meshes
 **************************************************************************************************/

#include "mesh/meshes.hpp"

#include <cmath>
#include <glad/glad.h>

Mesh Meshes::cube() {
    Mesh mesh(GL_TRIANGLES);

    /* Vertices' index
     *  0───1
     *  │╲  │╲
     *  │ 3───2
     *  4─│─5 │
     *   ╲│  ╲│
     *    7───6
     *
     * Faces & Template
     *      0┌─────┐1
     *       │  0  │
     * 0    3│ TOP │2    1     0
     * ┌─────┼─────┼─────┬─────┐
     * │  1  │  2  │  3  │  4  │
     * │ LEF │ FRO │ RIG │ BAC │
     * └─────┼─────┼─────┴─────┘
     * 4    7│  5  │6    5     4
     *       │ BOT │
     *      4└─────┘5
     */

    unsigned int faces[6][4]{
        {0, 3, 2, 1},
        {0, 4, 7, 3},
        {3, 7, 6, 2},
        {2, 6, 5, 1},
        {1, 5, 4, 0},
        {7, 4, 5, 6}
    };

    vec3 positions[8]{
        {-0.5f, 0.5f,  -0.5f},
        {0.5f,  0.5f,  -0.5f},
        {0.5f,  0.5f,  0.5f},
        {-0.5f, 0.5f,  0.5f},
        {-0.5f, -0.5f, -0.5f},
        {0.5f,  -0.5f, -0.5f},
        {0.5f,  -0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f}
    };

    vec3 normals[6]{
        {0.0f,  1.0f,  0.0f},
        {-1.0f, 0.0f,  0.0f},
        {0.0f,  0.0f,  1.0f},
        {1.0f,  0.0f,  0.0f},
        {0.0f,  0.0f,  -1.0f},
        {0.0f,  -1.0f, 0.0f}
    };

    for(int i = 0 ; i < 6 ; ++i) {
        mesh.addPosition(positions[faces[i][0]]);
        mesh.addNormal(normals[i]);
        mesh.addTexCoord(0.0f, 1.0f);

        mesh.addPosition(positions[faces[i][1]]);
        mesh.addNormal(normals[i]);
        mesh.addTexCoord(0.0f, 0.0f);

        mesh.addPosition(positions[faces[i][2]]);
        mesh.addNormal(normals[i]);
        mesh.addTexCoord(1.0f, 0.0f);

        mesh.addPosition(positions[faces[i][3]]);
        mesh.addNormal(normals[i]);
        mesh.addTexCoord(1.0f, 1.0f);

        mesh.addFace((i * 4), (i * 4) + 1, (i * 4) + 2, (i * 4) + 3);
    }

    return mesh;
}

Mesh Meshes::texturedCube() {
    Mesh mesh(GL_TRIANGLES);

    /* Vertices' index
     *  0───1
     *  │╲  │╲
     *  │ 3───2
     *  4─│─5 │
     *   ╲│  ╲│
     *    7───6
     *
     * Faces & Template
     *      0┌─────┐1
     *       │  0  │
     * 0    3│ TOP │2    1     0
     * ┌─────┼─────┼─────┬─────┐
     * │  1  │  2  │  3  │  4  │
     * │ LEF │ FRO │ RIG │ BAC │
     * └─────┼─────┼─────┴─────┘
     * 4    7│  5  │6    5     4
     *       │ BOT │
     *      4└─────┘5
     */

    unsigned int faces[6][4]{
        {0, 3, 2, 1},
        {0, 4, 7, 3},
        {3, 7, 6, 2},
        {2, 6, 5, 1},
        {1, 5, 4, 0},
        {7, 4, 5, 6}
    };

    vec3 positions[8]{
        {-0.5f, 0.5f,  -0.5f},
        {0.5f,  0.5f,  -0.5f},
        {0.5f,  0.5f,  0.5f},
        {-0.5f, 0.5f,  0.5f},
        {-0.5f, -0.5f, -0.5f},
        {0.5f,  -0.5f, -0.5f},
        {0.5f,  -0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f}
    };

    vec3 normals[6]{
        {0.0f,  1.0f,  0.0f},
        {-1.0f, 0.0f,  0.0f},
        {0.0f,  0.0f,  1.0f},
        {1.0f,  0.0f,  0.0f},
        {0.0f,  0.0f,  -1.0f},
        {0.0f,  -1.0f, 0.0f}
    };

    constexpr float n1_3 = 1.0f / 3.0f;
    constexpr float n2_3 = 2.0f / 3.0f;

    vec2 texCoords[6]{
        {0.25f, 1.0f},
        {0.00f, n2_3},
        {0.25f, n2_3},
        {0.50f, n2_3},
        {0.75f, n2_3},
        {0.25f, n1_3}
    };

    for(int i = 0 ; i < 6 ; ++i) {
        mesh.addPosition(positions[faces[i][0]]);
        mesh.addNormal(normals[i]);
        mesh.addTexCoord(texCoords[i]);

        mesh.addPosition(positions[faces[i][1]]);
        mesh.addNormal(normals[i]);
        mesh.addTexCoord(texCoords[i].x, texCoords[i].y - n1_3);

        mesh.addPosition(positions[faces[i][2]]);
        mesh.addNormal(normals[i]);
        mesh.addTexCoord(texCoords[i].x + 0.25f, texCoords[i].y - n1_3);

        mesh.addPosition(positions[faces[i][3]]);
        mesh.addNormal(normals[i]);
        mesh.addTexCoord(texCoords[i].x + 0.25f, texCoords[i].y);

        mesh.addFace((i * 4), (i * 4) + 1, (i * 4) + 2, (i * 4) + 3);
    }

    return mesh;
}

Mesh Meshes::wireframeCube() {
    Mesh mesh(GL_LINES);

    mesh.addPosition(-0.5f, 0.5f, -0.5f);
    mesh.addPosition(0.5f, 0.5f, -0.5f);
    mesh.addPosition(0.5f, 0.5f, 0.5f);
    mesh.addPosition(-0.5f, 0.5f, 0.5f);
    mesh.addPosition(-0.5f, -0.5f, -0.5f);
    mesh.addPosition(0.5f, -0.5f, -0.5f);
    mesh.addPosition(0.5f, -0.5f, 0.5f);
    mesh.addPosition(-0.5f, -0.5f, 0.5f);

    /* Vertices' index
     *  0───1
     *  │╲  │╲
     *  │ 3───2
     *  4─│─5 │
     *   ╲│  ╲│
     *    7───6
     */

    unsigned int lines[12][2]{
        {0, 1},
        {0, 3},
        {0, 4},
        {1, 2},
        {1, 5},
        {2, 3},
        {2, 6},
        {3, 7},
        {4, 5},
        {4, 7},
        {5, 6},
        {6, 7}
    };

    for(int i = 0 ; i < 12 ; ++i) {
        mesh.addIndex(lines[i][0]);
        mesh.addIndex(lines[i][1]);
    }

    return mesh;
}

Mesh Meshes::plainCube() {
    Mesh mesh(GL_TRIANGLES);

    /* Vertices' index
     *  0───1
     *  │╲  │╲
     *  │ 3───2
     *  4─│─5 │
     *   ╲│  ╲│
     *    7───6
     *
     * Faces & Template
     *      0┌─────┐1
     *       │  0  │
     * 0    3│ TOP │2    1     0
     * ┌─────┼─────┼─────┬─────┐
     * │  1  │  2  │  3  │  4  │
     * │ LEF │ FRO │ RIG │ BAC │
     * └─────┼─────┼─────┴─────┘
     * 4    7│  5  │6    5     4
     *       │ BOT │
     *      4└─────┘5
     */

    unsigned int faces[6][4]{
        {0, 3, 2, 1},
        {0, 4, 7, 3},
        {3, 7, 6, 2},
        {2, 6, 5, 1},
        {1, 5, 4, 0},
        {7, 4, 5, 6}
    };

    vec3 positions[8]{
        {-0.5f, 0.5f,  -0.5f},
        {0.5f,  0.5f,  -0.5f},
        {0.5f,  0.5f,  0.5f},
        {-0.5f, 0.5f,  0.5f},
        {-0.5f, -0.5f, -0.5f},
        {0.5f,  -0.5f, -0.5f},
        {0.5f,  -0.5f, 0.5f},
        {-0.5f, -0.5f, 0.5f}
    };

    for(int i = 0 ; i < 6 ; ++i) {
        mesh.addPosition(positions[faces[i][0]]);
        mesh.addTexCoord(0.0f, 1.0f);

        mesh.addPosition(positions[faces[i][1]]);
        mesh.addTexCoord(0.0f, 0.0f);

        mesh.addPosition(positions[faces[i][2]]);
        mesh.addTexCoord(1.0f, 0.0f);

        mesh.addPosition(positions[faces[i][3]]);
        mesh.addTexCoord(1.0f, 1.0f);

        mesh.addFace((i * 4), (i * 4) + 1, (i * 4) + 2, (i * 4) + 3);
    }

    return mesh;
}

Mesh Meshes::cubemap() {
    Mesh mesh(GL_TRIANGLES);

    /* Vertices' index
     *  0───1
     *  │╲  │╲
     *  │ 3───2
     *  4─│─5 │
     *   ╲│  ╲│
     *    7───6
     *
     * Faces & Template
     *      0┌─────┐1
     *       │  0  │
     * 0    3│ TOP │2    1     0
     * ┌─────┼─────┼─────┬─────┐
     * │  1  │  2  │  3  │  4  │
     * │ LEF │ FRO │ RIG │ BAC │
     * └─────┼─────┼─────┴─────┘
     * 4    7│  5  │6    5     4
     *       │ BOT │
     *      4└─────┘5
     */

    unsigned int faces[6][4]{
        {1, 2, 3, 0},
        {3, 7, 4, 0},
        {2, 6, 7, 3},
        {1, 5, 6, 2},
        {0, 4, 5, 1},
        {6, 5, 4, 7}
    };

    vec3 positions[8]{
        {-1.0f, 1.0f,  -1.0f},
        {1.0f,  1.0f,  -1.0f},
        {1.0f,  1.0f,  1.0f},
        {-1.0f, 1.0f,  1.0f},
        {-1.0f, -1.0f, -1.0f},
        {1.0f,  -1.0f, -1.0f},
        {1.0f,  -1.0f, 1.0f},
        {-1.0f, -1.0f, 1.0f}
    };

    for(int i = 0 ; i < 6 ; ++i) {
        mesh.addPosition(positions[faces[i][0]]);
        mesh.addPosition(positions[faces[i][1]]);
        mesh.addPosition(positions[faces[i][2]]);
        mesh.addPosition(positions[faces[i][3]]);

        mesh.addFace((i * 4), (i * 4) + 1, (i * 4) + 2, (i * 4) + 3);
    }

    return mesh;
}

Mesh Meshes::grid(float size, int divisions) {
    Mesh mesh(GL_LINES);

    float square = -size / 2.0f;
    const float squareSize = size / divisions;

    for(int i = 0 ; i <= divisions ; ++i) {
        mesh.addPosition(square, 0.0f, -size / 2.0f);
        mesh.addPosition(square, 0.0f, size / 2.0f);

        mesh.addPosition(-size / 2.0f, 0.0f, square);
        mesh.addPosition(size / 2.0f, 0.0f, square);

        square += squareSize;
    }

    return mesh;
}

Mesh Meshes::planeGrid(float size, int divisions) {
    Mesh mesh(GL_TRIANGLES);

    const float halfSize = -size / 2.0f;
    const float squareSize = size / divisions;
    float squareX = halfSize, squareZ = halfSize;

    for(int i = 0 ; i <= divisions ; ++i) {
        for(int j = 0 ; j <= divisions ; ++j) {
            mesh.addPosition(squareX, 0.0f, squareZ);
            mesh.addNormal(0.0f, 1.0f, 0.0f);

            squareX += squareSize;
        }

        squareX = halfSize;
        squareZ += squareSize;
    }

    auto index = [&](int x, int y) -> int {
        return x + y * (divisions + 1);
    };

    for(int i = 0 ; i < divisions ; ++i) {
        for(int j = 0 ; j < divisions ; ++j) {
            mesh.addFace(index(i, j),
                         index(i, j + 1),
                         index(i + 1, j + 1),
                         index(i + 1, j));
        }
    }

    return mesh;
}

Mesh Meshes::axes(float size) {
    Mesh mesh(GL_LINES);

    const vec3 axes[3]{
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    };

    for(const vec3& p: axes) {
        mesh.addPosition(0.0f, 0.0f, 0.0f);
        mesh.addColor(p);

        mesh.addPosition(size * p);
        mesh.addColor(p);
    }

    return mesh;
}

Mesh Meshes::sphere(int divTheta, int divPhi) {
    Mesh mesh(GL_TRIANGLES);

    const double thetaStep = M_PI / divTheta;
    const double phiStep = 2.0f * M_PI / divPhi;

    double theta = -M_PI_2 + thetaStep;
    double phi = 0.0f;

    vec3 point;
    for(int i = 0 ; i < divTheta - 1 ; ++i) {
        phi = 0.0;

        for(int j = 0 ; j < divPhi ; ++j) {
            point.x = cos(theta) * cos(phi);
            point.y = sin(theta);
            point.z = cos(theta) * sin(phi);

            mesh.addPosition(point);
            mesh.addNormal(point);

            phi += phiStep;
        }

        theta += thetaStep;
    }

    auto index = [&](int column, int row) -> int {
        return row + column * divPhi;
    };

    for(int i = 0 ; i < divTheta - 2 ; ++i) {
        for(int j = 0 ; j < divPhi ; ++j) {
            mesh.addFace(
                index(i, j),
                index(i + 1, j),
                index(i + 1, (j + 1) % divPhi),
                index(i, (j + 1) % divPhi)
            );
        }
    }

    mesh.addPosition(0.0f, -1.0f, 0.0f);
    mesh.addNormal(0.0f, -1.0f, 0.0f);

    mesh.addPosition(0.0f, 1.0f, 0.0f);
    mesh.addNormal(0.0f, 1.0f, 0.0f);

    for(int i = 0 ; i < divPhi ; ++i) {
        mesh.addTriangle(
            index(divTheta - 1, 0),
            index(0, i),
            index(0, (i + 1) % divPhi)
        );

        mesh.addTriangle(
            index(divTheta - 2, i),
            index(divTheta - 1, 0) + 1,
            index(divTheta - 2, (i + 1) % divPhi)
        );
    }

    return mesh;
}

Mesh Meshes::texturedSphere(int divTheta, int divPhi) {
    Mesh mesh(GL_TRIANGLES);

    const double thetaStep = M_PI / divTheta;
    const double phiStep = 2.0 * M_PI / divPhi;

    double theta = -M_PI_2 + thetaStep;
    double phi = 0.0;

    vec3 point;
    for(int i = 0 ; i < divTheta ; ++i) {
        phi = 0.0;

        for(int j = 0 ; j <= divPhi ; ++j) {
            point.x = cos(theta) * cos(phi);
            point.y = sin(theta);
            point.z = cos(theta) * sin(phi);

            mesh.addPosition(point);
            mesh.addNormal(point);
            mesh.addTexCoord(static_cast<float>(j) / divPhi, 0.5 + point.y / 2.0);

            phi += phiStep;
        }

        theta += thetaStep;
    }

    auto index = [&](int column, int row) -> int {
        return row + column * (divPhi + 1);
    };

    for(int i = 0 ; i < divTheta - 1 ; ++i) {
        for(int j = 0 ; j < divPhi ; ++j) {
            mesh.addFace(
                index(i, j),
                index(i + 1, j),
                index(i + 1, j + 1),
                index(i, j + 1)
            );
        }
    }

    return mesh;
}

Mesh Meshes::plane(float size) {
    Mesh mesh(GL_TRIANGLES);

    size /= 2;

    mesh.addPosition(-size, 0.0f, size);
    mesh.addTexCoord(0.0f, size);

    mesh.addPosition(size, 0.0f, size);
    mesh.addTexCoord(size, size);

    mesh.addPosition(size, 0.0f, -size);
    mesh.addTexCoord(size, 0.0f);

    mesh.addPosition(-size, 0.0f, -size);
    mesh.addTexCoord(0.0f, 0.0f);

    mesh.addFace(0, 1, 2, 3);

    return mesh;
}

Mesh Meshes::chunk() {
    Mesh mesh(GL_PATCHES);

    mesh.addPosition(-0.5f, 0.0f, 0.5f);
    mesh.addPosition(0.5f, 0.0f, 0.5f);
    mesh.addPosition(0.5f, 0.0f, -0.5f);
    mesh.addPosition(-0.5f, 0.0f, -0.5f);

    return mesh;
}

Mesh Meshes::tessGrid(float size, int divisions) {
    Mesh mesh(GL_PATCHES);

    const float halfSize = -size / 2.0f;
    const float squareSize = size / divisions;
    float squareX = halfSize, squareZ = halfSize;

    for(int i = 0 ; i <= divisions ; ++i) {
        for(int j = 0 ; j <= divisions ; ++j) {
            mesh.addPosition(squareX, 0.0f, squareZ);

            squareX += squareSize;
        }

        squareX = halfSize;
        squareZ += squareSize;
    }

    auto index = [&](int x, int y) -> int {
        return x + y * (divisions + 1);
    };

    for(int i = 0 ; i < divisions ; ++i) {
        for(int j = 0 ; j < divisions ; ++j) {
            mesh.addIndex(index(i, j));
            mesh.addIndex(index(i, j + 1));
            mesh.addIndex(index(i + 1, j + 1));
            mesh.addIndex(index(i + 1, j));
        }
    }

    return mesh;
}

Mesh Meshes::nplane(float size) {
    Mesh mesh(GL_TRIANGLES);

    size /= 2;

    mesh.addPosition(-size, 0.0f, size);
    mesh.addNormal(0.0f, 1.0f, 0.0f);
    mesh.addTexCoord(0.0f, size);

    mesh.addPosition(size, 0.0f, size);
    mesh.addNormal(0.0f, 1.0f, 0.0f);
    mesh.addTexCoord(size, size);

    mesh.addPosition(size, 0.0f, -size);
    mesh.addNormal(0.0f, 1.0f, 0.0f);
    mesh.addTexCoord(size, 0.0f);

    mesh.addPosition(-size, 0.0f, -size);
    mesh.addNormal(0.0f, 1.0f, 0.0f);
    mesh.addTexCoord(0.0f, 0.0f);

    mesh.addFace(0, 1, 2, 3);

    return mesh;
}

Mesh Meshes::screen() {
    Mesh mesh(GL_TRIANGLES);

    mesh.addPosition(-1.0f, 1.0f, 0.0f);
    mesh.addPosition(-1.0f, -1.0f, 0.0f);
    mesh.addPosition(1.0f, -1.0f, 0.0f);
    mesh.addPosition(1.0f, 1.0f, 0.0f);

    mesh.addFace(0, 1, 2, 3);

    return mesh;
}
