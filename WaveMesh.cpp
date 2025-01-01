//
// Created by Dylan Beaumont on 31/12/2024.
//

#include "WaveMesh.h"

#include <GLFW/glfw3.h>

#include "Shader.h"
#include "glad/glad.h"

WaveMesh::WaveMesh(int maxVertices, int numVertices, float (*waveEquation)(float, float))
{
    // We don't want to create a new vertex vector and assign memory each time it changes, so instead we create it once
    // at the maximum possible size, and then only render part of it.
    this->waveEquation = waveEquation;
    this->numVertices = numVertices;
    this->maxVertices = maxVertices;
    vertices.resize(maxVertices*maxVertices);
    const int maxIndices = (maxVertices-1)*(maxVertices-1)*6;
    numIndices = (numVertices-1)*(numVertices-1)*6;
    indices.resize(maxIndices);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    // bind the index array now, as this won't change unless we resize the mesh.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(maxVertices * maxVertices * sizeof(Vertex)), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(maxIndices * sizeof(unsigned int)), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(numIndices * sizeof(unsigned int)), indices.data());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, texCoords)));
    glEnableVertexAttribArray(2);
    updateVertices();
    updateIndices();
}

void WaveMesh::updateVertices() {
    const auto fNumVertices = static_cast<float>(numVertices);
    for (int i=0; i<numVertices; i++) {
        for (int j=0; j<numVertices; j++) {
            const auto fI = static_cast<float>(i);
            const auto fJ = static_cast<float>(j);
            // positions
            vertices[(i*numVertices + j)].position.x = -2.0f + 4*fI/fNumVertices;
            vertices[(i*numVertices + j)].position.y = 0.0; // static_cast<float>(0.05*std::sin(10*fI/fNumVertices + 1.5*glfwGetTime()) + 0.025*std::cos(20*fJ/fNumVertices + glfwGetTime()) + 0.05*std::cos(10*fJ/fNumVertices + glfwGetTime()));
            vertices[(i*numVertices + j)].position.z = -2.0f + 4*fJ/fNumVertices;
            // normals
            vertices[(i*numVertices + j)].normal.x =  0.0f;
            vertices[(i*numVertices + j)].normal.y =  1.0f;
            vertices[(i*numVertices + j)].normal.z =  0.0f;

            vertices[(i*numVertices + j)].texCoords.x =  fI/fNumVertices;
            vertices[(i*numVertices + j)].texCoords.y =  fJ/fNumVertices;
        }
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(numVertices * numVertices * sizeof(Vertex)), vertices.data());
}

void WaveMesh::updateIndices() {
    int pos = 0;
    for (int i=0; i<numVertices-1; i++) {
        for (int j=1; j<numVertices; j++) {
            // triangle 1
            indices[pos] = i*numVertices + j-1;
            indices[pos+1] = i*numVertices + j;
            indices[pos+2] = (i+1)*numVertices + j-1;
            // triangle 2
            indices[pos+3] = i*numVertices + j;
            indices[pos+4] = (i+1)*numVertices + j;
            indices[pos+5] = (i+1)*numVertices + j-1;
            pos += 6;
        }
    }
    numIndices = (numVertices-1)*(numVertices-1)*6;
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(numIndices * sizeof(unsigned int)), indices.data());
}

void WaveMesh::draw(const Shader *shader) const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(numIndices), GL_UNSIGNED_INT, nullptr);
}

