
#include "Line.hpp"

#include <GLAD/src/glad.h>

#include "../../resource/ResourceShader.hpp"

#include <glm/glm.hpp>

Line::Line(glm::vec3 startPoint, glm::vec3 direction, float range)
{
    auto endPoint = startPoint + direction * range;
    vertexs[0] = startPoint.x;
    vertexs[1] = startPoint.y;
    vertexs[2] = startPoint.z;
    vertexs[3] = endPoint.x;
    vertexs[4] = endPoint.y;
    vertexs[5] = endPoint.z;
}
Line::Line(glm::vec3 startPoint, glm::vec3 endPoint)
{
    vertexs[0] = startPoint.x;
    vertexs[1] = startPoint.y;
    vertexs[2] = startPoint.z;
    vertexs[3] = endPoint.x;
    vertexs[4] = endPoint.y;
    vertexs[5] = endPoint.z;
}
Line::~Line()
{
    drop();
}
void Line::drop()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Line::setup(glm::vec4 color, float lineWidth)
{
    this->color = color;
    this->lineWidth = lineWidth;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs), vertexs, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    
    glEnableVertexAttribArray(0);
}

void Line::updateLine(glm::vec3 startPoint, glm::vec3 direction, float range)
{
    auto endPoint = startPoint + direction * range;
    vertexs[0] = startPoint.x;
    vertexs[1] = startPoint.y;
    vertexs[2] = startPoint.z;
    vertexs[3] = endPoint.x;
    vertexs[4] = endPoint.y;
    vertexs[5] = endPoint.z;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexs), vertexs);
}

void Line::draw(ResourceShader &shader)
{
    shader.use();
    shader.setVec4("color", color);
    shader.setMat4("model", glm::mat4(1.0f));
    glLineWidth(lineWidth);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}