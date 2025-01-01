//
// Created by Dylan Beaumont on 31/12/2024.
//

#ifndef WAVEMESH_H
#define WAVEMESH_H
#include <vector>
#include <glm/vec3.hpp>

#include "Shader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class WaveMesh {

public:
    // function pointer to the wave equation we want to use.
    float (*waveEquation)(float, float);
    int maxVertices;
    int numVertices;
    int numIndices;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO{}, VBO{}, EBO{};


    WaveMesh(int maxVertices, int numVertices, float (*waveEquation)(float, float));

    void updateVertices();
    void updateIndices();
    void draw(const Shader *shader) const;
    //void updateWaveEquation(float (*waveEquation)(float, float));
    //void updateNumVertices(unsigned int numVertices);
};



#endif //WAVEMESH_H
