#pragma once

#include "Vertex.hpp"

#include <vector>

#include <glm/mat4x4.hpp>

struct ResourceShader;
struct ResourceMaterial;
struct ResourceTexture;
struct ResourceModel;
struct LightParams;
struct SGEngine;
struct Mesh
{

    friend ResourceModel;
    friend SGEngine;

    explicit Mesh() = default;
    Mesh(std::vector<Vertex> vertex, std::vector<unsigned int> index, glm::vec3 position, glm::vec3 dimensions);
    ~Mesh();

    void drop();

    void setup(bool HasBones = false);
    void setupInstancing(std::vector<glm::mat4> &tranforms);

    void draw(ResourceShader &shader, glm::mat4 matrix);

    void updateInstances(std::vector<glm::mat4> &newTransforms);

    void drawInstances(ResourceShader &shader);

    constexpr glm::vec3 getPosition() const noexcept { return position; };
    constexpr glm::vec3 getDimension() const noexcept { return dimensions; };

    void setTexture(ResourceTexture *texture);

    std::vector<Vertex> vertex;
    std::vector<unsigned int> index;
    ResourceMaterial *material{nullptr};

    void addLightToRender(LightParams &light);

private:
    unsigned int VAO, VBO, EBO, bufferInstances;

    std::vector<glm::mat4> Tranforms4Instancing{};
    std::vector<glm::mat4> currentTransforms{};
    std::size_t BufferInstanceSize;


    std::vector<LightParams *> lights;

    glm::vec3 position{0.0f};
    glm::vec3 dimensions{0.0f};

    void updateMaterial(ResourceShader &shader);

    void updateLights(ResourceShader &shader);
};