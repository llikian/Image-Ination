/***************************************************************************************************
 * @file  Mesh.cpp
 * @brief Implementation of the Mesh class
 **************************************************************************************************/

#include "mesh/Mesh.hpp"

#include <glad/glad.h>

Mesh::Mesh(unsigned int primitive)
    : primitive(primitive),
      shouldBind(true),
      attributes(0b00000001) {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

Mesh::Mesh(const Mesh& mesh)
    : primitive(mesh.getPrimitive()),
      shouldBind(true),
      attributes(mesh.getAttributes()),
      data(*mesh.getData()),
      indices(*mesh.getIndices()) {

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
}

Mesh& Mesh::operator =(const Mesh& mesh) {
    if(&mesh == this) {
        return *this;
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    primitive = mesh.getPrimitive();
    shouldBind = true;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    attributes = mesh.getAttributes();
    data = *mesh.getData();
    indices = *mesh.getIndices();

    return *this;
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::draw() {
    if(shouldBind) {
        bindBuffers();
        shouldBind = false;
    }

    glBindVertexArray(VAO);

    int shader;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shader);
    glUniform1ui(glGetUniformLocation(shader, "attributes"), static_cast<unsigned int>(attributes));

    if(indices.empty()) {
        glDrawArrays(primitive, 0, data.size() / getStride());
    } else {
        glDrawElements(primitive, indices.size(), GL_UNSIGNED_INT, nullptr);
    }
}

void Mesh::addPosition(float x, float y, float z) {
    data.push_back(x);
    data.push_back(y);
    data.push_back(z);
}

void Mesh::addPosition(const vec3& position) {
    data.push_back(position.x);
    data.push_back(position.y);
    data.push_back(position.z);
}

void Mesh::addNormal(float x, float y, float z) {
    attributes |= 0b00000010;

    data.push_back(x);
    data.push_back(y);
    data.push_back(z);
}

void Mesh::addNormal(const vec3& normal) {
    attributes |= 0b00000010;

    data.push_back(normal.x);
    data.push_back(normal.y);
    data.push_back(normal.z);
}

void Mesh::addTexCoord(float x, float y) {
    attributes |= 0b00000100;

    data.push_back(x);
    data.push_back(y);
}

void Mesh::addTexCoord(const vec2& texCoord) {
    attributes |= 0b00000100;

    data.push_back(texCoord.x);
    data.push_back(texCoord.y);
}

void Mesh::addColor(float r, float g, float b) {
    attributes |= 0b00001000;

    data.push_back(r);
    data.push_back(g);
    data.push_back(b);
}

void Mesh::addColor(const vec3& color) {
    attributes |= 0b00001000;

    data.push_back(color.x);
    data.push_back(color.y);
    data.push_back(color.z);
}

void Mesh::addIndex(unsigned int index) {
    indices.push_back(index);
}

void Mesh::addTriangle(unsigned int top, unsigned int left, unsigned int right) {
    indices.push_back(top);
    indices.push_back(left);
    indices.push_back(right);
}

void Mesh::addFace(unsigned int topL, unsigned int bottomL,
                   unsigned int bottomR, unsigned int topR) {

    // First Triangle
    indices.push_back(topL);
    indices.push_back(bottomL);
    indices.push_back(bottomR);

    // Second Triangle
    indices.push_back(topL);
    indices.push_back(bottomR);
    indices.push_back(topR);
}

unsigned int Mesh::getPrimitive() const {
    return primitive;
}

u_int8_t Mesh::getAttributes() const {
    return attributes;
}

const std::vector<float>* Mesh::getData() const {
    return &data;
}

const std::vector<unsigned int>* Mesh::getIndices() const {
    return &indices;
}

void Mesh::bindBuffers() {
    const unsigned int stride = getStride() * sizeof(float);
    int offset = 0;

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, reinterpret_cast<void*>(offset));
    glEnableVertexAttribArray(0);
    offset += 3 * sizeof(float);

    if((attributes >> 1) & 1) { // Normal
        glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, reinterpret_cast<void*>(offset));
        glEnableVertexAttribArray(1);
        offset += 3 * sizeof(float);
    }

    if((attributes >> 2) & 1) { // Texture Coordinates
        glVertexAttribPointer(2, 2, GL_FLOAT, false, stride, reinterpret_cast<void*>(offset));
        glEnableVertexAttribArray(2);
        offset += 2 * sizeof(float);
    }

    if((attributes >> 3) & 1) { // Color
        glVertexAttribPointer(3, 3, GL_FLOAT, false, stride, reinterpret_cast<void*>(offset));
        glEnableVertexAttribArray(3);
        offset += 3 * sizeof(float);
    }

    if(!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(unsigned int),
                     indices.data(), GL_STATIC_DRAW);
    }
}

unsigned int Mesh::getStride() const {
    unsigned int stride = 3;

    if((attributes >> 1) & 1) { // Normal
        stride += 3;
    }

    if((attributes >> 2) & 1) { // Texture Coordinates
        stride += 2;
    }

    if((attributes >> 3) & 1) { // Color
        stride += 3;
    }

    return stride;
}
