
#include "TBoundingBox.hpp"

#include <GLAD/src/glad.h>
#include <engines/graphic/resource/ResourceShader.hpp>

#include <iostream>

TBoundingBox::TBoundingBox(glm::vec3 size, ResourceShader *shader) : size(size), initialSize(size)
{
    this->shader = shader;
    for (auto i = 0; i < 8; i++)
    {
        glm::vec3 v;
        // operator ternary
        v.x = ((i & 1) == 0 ? -1 : 1) * size.x / 2.0f;
        v.y = ((i & 2) == 0 ? -1 : 1) * size.y / 2.0f;
        v.z = ((i & 4) == 0 ? -1 : 1) * size.z / 2.0f;
        vertexs.emplace_back(v);
    }

    // Calculate Faces
    for (std::size_t i = 0; i < 6; i++)
    {
        std::vector<unsigned int> indexs_face;
        for (std::size_t j = 0; j < 4; j++)
        {
            indexs_face.emplace_back(indexs[i * 4 + j]);
        }

        // Calulate center
        glm::vec3 auxAB = vertexs[indexs_face[1]] - vertexs[indexs_face[0]];
        glm::vec3 auxAC = vertexs[indexs_face[3]] - vertexs[indexs_face[0]];

        glm::vec3 center = vertexs[indexs_face[0]] + auxAB / 2.f + auxAC / 2.f;

        // Calulate normal
        glm::vec3 normal = glm::normalize(center);

        faces.emplace_back(indexs_face, normal, center);
    }
    globalVertexs = vertexs;
}

TBoundingBox::~TBoundingBox()
{
    drop();
}

void TBoundingBox::drop()
{
    if (initialized)
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        initialized = false;
    }
}

void TBoundingBox::setup(glm::vec4 color)
{
    if (!initialized)
    {
        this->color = color;
        // Create and link VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Create and link Vertex Buffer Objects (VBO) and Element Buffer Object (EBO)
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // Vertex
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexs.size() * sizeof(glm::vec3)), &vertexs[0], GL_STATIC_DRAW);

        // Index
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexsLines.size() * sizeof(unsigned int)), &indexsLines[0], GL_STATIC_DRAW);

        // Vertex Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
        glEnableVertexAttribArray(0);

        initialized = true;
    }
}

// Call when need actual position
void TBoundingBox::updateValues(glm::mat4 matrixTransform)
{
    // Asi calculamos los nuevos vertices solo cuando cambie
    if (lastMatrix != matrixTransform)
    {
        lastMatrix = matrixTransform;

        globalPos = glm::vec3(matrixTransform[3]);

        glm::vec3 scale = glm::vec3(glm::length(matrixTransform[0]), glm::length(matrixTransform[1]), glm::length(matrixTransform[2]));

        if (this->scale != scale)
        {
            size = initialSize * scale;
            this->scale = scale;
        }

        for (std::size_t i = 0; i < vertexs.size(); i++)
            globalVertexs[i] = glm::vec3(matrixTransform * glm::vec4(vertexs[i], 1));

        for (auto &face : faces)
        {
            glm::vec3 auxAB = globalVertexs[face.index[1]] - globalVertexs[face.index[0]];
            glm::vec3 auxAC = globalVertexs[face.index[3]] - globalVertexs[face.index[0]];

            face.center = globalVertexs[face.index[0]] + auxAB / 2.f + auxAC / 2.f;

            face.normal = glm::normalize(face.center - globalPos);
        }
    }
}

void TBoundingBox::update()
{
    // Draw Collision
    if (initialized)
    {
        shader->use();
        shader->setMat4("model", matrix);
        shader->setVec4("color", color);
        glLineWidth(3.0f);
        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, GLsizei(indexsLines.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}