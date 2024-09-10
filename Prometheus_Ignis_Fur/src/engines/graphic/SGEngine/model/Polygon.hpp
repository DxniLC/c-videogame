#pragma once

#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct ResourceShader;
struct Polygon
{
    explicit Polygon(std::vector<glm::vec3> globalVertexs, glm::vec3 globalCenterPos, glm::vec3 normal,float sphereRange);
    ~Polygon();
    void drop(); 

    void setup(glm::vec4 color = glm::vec4(1, 0, 0, 1),float lineWidth = 3.0f);
    void draw(ResourceShader &shader);

    std::vector<glm::vec3> globalVertexs{};
    glm::vec3 globalCenterPos{};
    glm::vec3 normal{};
    float polygonRange{10.f};

private:

    bool Initialized{false};

    unsigned int VAO{}, VBO{};
    glm::vec4 color{0.0f};
    float lineWidth{5.0f};

};
