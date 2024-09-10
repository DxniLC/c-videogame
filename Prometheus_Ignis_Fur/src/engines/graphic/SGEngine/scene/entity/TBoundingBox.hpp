#pragma once

#include "TEntity.hpp"
#include <vector>

#include <engines/graphic/SGEngine/model/Face.hpp>

struct ResourceShader;
struct TBoundingBox : public TEntity
{
    explicit TBoundingBox(glm::vec3 size, ResourceShader *shader);

    ~TBoundingBox();

    void update() override final;

    void updateValues(glm::mat4 matrixTransform);
    
    void setup(glm::vec4 color = glm::vec4(1, 0, 0, 1));

    void drop();

    // TODO: Ver como gestionar todo el tema de colisiones y ver cual es la mejor forma de hacer los calculos

    glm::vec3 size{};
    std::vector<glm::vec3> vertexs{};
    std::vector<Face> faces{};
    std::vector<glm::vec3> globalVertexs{};
    glm::vec3 globalPos{};

private:
    glm::vec3 initialSize{};
    glm::vec3 scale{};
    ResourceShader *shader{nullptr};
    bool initialized{false};
    const std::vector<unsigned int> indexs{0, 1, 3, 2,
                                           4, 6, 7, 5,
                                           1, 5, 7, 3,
                                           0, 2, 6, 4,
                                           2, 3, 7, 6,
                                           0, 4, 5, 1};
    const std::vector<unsigned int> indexsLines{0, 1, 1, 3,
                                                3, 2, 2, 0,
                                                0, 4, 4, 5,
                                                5, 1, 5, 7,
                                                4, 6, 6, 7,
                                                2, 6, 3, 7};

    glm::mat4 lastMatrix{1.0f};
    glm::vec4 color{};
    unsigned int VAO, VBO, EBO;
};