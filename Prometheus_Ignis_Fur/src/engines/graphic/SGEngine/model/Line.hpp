#pragma once

#pragma once

#include "Vertex.hpp"

#include <vector>

#include <glm/mat4x4.hpp>

struct ResourceShader;
struct Line
{
    explicit Line() = default;
    Line(glm::vec3 startPoint, glm::vec3 direction, float range);
    Line(glm::vec3 startPoint, glm::vec3 endPoint);
    ~Line();
    void drop();
    void draw(ResourceShader &shader);

    void updateLine(glm::vec3 startPoint, glm::vec3 direction, float range);
    void setup(glm::vec4 color = glm::vec4(0,1,0,1), float lineWidth = 3.0f);

private:
    unsigned int VAO, VBO;
    float vertexs[6];
    glm::vec4 color;
    float lineWidth{3.0f};
};