
#include "Polygon.hpp"

#include <GLAD/src/glad.h>
#include <engines/graphic/resource/ResourceShader.hpp>

#include <iostream>

Polygon::Polygon(std::vector<glm::vec3> globalVertexs, glm::vec3 globalCenterPos, glm::vec3 normal, float sphereRange)
    : globalVertexs{globalVertexs}, globalCenterPos{globalCenterPos}, normal{normal}, polygonRange{sphereRange}
{
}

Polygon::~Polygon()
{
    if (Initialized)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        Initialized = false;
    }
}

void Polygon::drop()
{
    if (Initialized)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        Initialized = false;
    }
}

void Polygon::setup(glm::vec4 color, float width)
{

    this->color = color;
    this->lineWidth = width;
    // Crate and link VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create and link Vertex Buffer Objects (VBO) and Element Buffer Object (EBO)
    glGenBuffers(1, &VBO);

    // Vertex
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(globalVertexs.size() * sizeof(glm::vec3)), &globalVertexs[0], GL_STATIC_DRAW);

    // Vertex Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(0);

    Initialized = true;
}

void Polygon::draw(ResourceShader &shader)
{
    shader.use();
    shader.setVec4("color", color);
    shader.setMat4("model", glm::mat4(1.0f));
    glLineWidth(lineWidth);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_LOOP, 0, GLsizei(globalVertexs.size()));
}