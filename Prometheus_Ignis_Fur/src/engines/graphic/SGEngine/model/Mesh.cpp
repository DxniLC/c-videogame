
#include "Mesh.hpp"

#include <GLAD/src/glad.h>

#include <iostream>

#include <engines/graphic/resource/ResourceShader.hpp>
#include <engines/graphic/resource/ResourceTexture.hpp>
#include <engines/graphic/resource/ResourceMaterial.hpp>

#include "../scene/entity/light/TLight.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Mesh::Mesh(std::vector<Vertex> vertex, std::vector<unsigned int> index, glm::vec3 position, glm::vec3 dimensions)
    : vertex{vertex}, index{index}, position{position}, dimensions{dimensions} {}

Mesh::~Mesh()
{
    drop();
}

void Mesh::drop()
{
    // std::cout << "\nDROP MESH\n";
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &bufferInstances);
}

void Mesh::setup(bool HasBones)
{
    // Create and link VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create and link Vertex Buffer Objects (VBO) and Element Buffer Object (EBO)
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Vertex
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertex.size() * sizeof(Vertex)), &vertex[0], GL_STATIC_DRAW);

    // Index
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(index.size() * sizeof(unsigned int)), &index[0], GL_STATIC_DRAW);

    // Vertex Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    // Vertex Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));

    // Vertex Texture Coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoord));

    if (HasBones)
    {
        // Bones IDs
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, BoneIDs));

        // Bones Weights
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Weight));
    }

    glBindVertexArray(0);
}

void Mesh::setupInstancing(std::vector<glm::mat4> &transforms)
{
    Tranforms4Instancing = transforms;
    currentTransforms = transforms;

    BufferInstanceSize = Tranforms4Instancing.size() * sizeof(glm::mat4);

    setup();

    glGenBuffers(1, &bufferInstances);
    glBindBuffer(GL_ARRAY_BUFFER, bufferInstances);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(BufferInstanceSize), &currentTransforms[0], GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(1 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void *)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
}

void Mesh::draw(ResourceShader &shader, glm::mat4 matrix)
{

    updateMaterial(shader);

    updateLights(shader);

    glBindVertexArray(VAO);
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(matrix)));
    shader.setMat4("model", matrix);
    shader.setMat3("NormalMatrix", normalMatrix);

    glDrawElements(GL_TRIANGLES, GLsizei(index.size()), GL_UNSIGNED_INT, 0);

    // Desenlazamos VAO, Buffer del array y Buffer de elementos para que el siguiente funcione correctamente
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::drawInstances(ResourceShader &shader)
{

    updateMaterial(shader);

    updateLights(shader);

    glBindVertexArray(VAO);

    glDrawElementsInstanced(GL_TRIANGLES, GLsizei(index.size()), GL_UNSIGNED_INT, 0, GLsizei(currentTransforms.size()));

    // Desenlazamos VAO, Buffer del array y Buffer de elementos para que el siguiente funcione correctamente
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::updateInstances(std::vector<glm::mat4> &newTransforms)
{
    currentTransforms = newTransforms;
    glBindBuffer(GL_ARRAY_BUFFER, bufferInstances);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(BufferInstanceSize), &currentTransforms[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::setTexture(ResourceTexture *texture)
{
    if (material)
    {
        material->diffuseMap = texture;
    }
    else
    {
        std::cout << "\n **** EL OBJETO NO TIENE MATERIAL **** \n";
    }
}

void Mesh::updateMaterial(ResourceShader &shader)
{
    // Desactivamos Texturas
    glBindTexture(GL_TEXTURE_2D, 0);

    // Material
    if (material)
    {
        if (material->diffuseMap)
        {
            glActiveTexture(GL_TEXTURE0);
            shader.setInt("material.diffuseTexture", 0);
            shader.setBool("material.hasDiffuseTexture", true);
            glBindTexture(GL_TEXTURE_2D, material->diffuseMap->id);
        }
        else
        {
            shader.setBool("material.hasDiffuseTexture", false);
            shader.setVec4("material.diffuse", material->diffuse);
        }

        if (material->lightMap)
        {
            glActiveTexture(GL_TEXTURE0 + 1);
            shader.setInt("material.lightTexture", 1);
            shader.setBool("material.hasLightTexture", true);
            glBindTexture(GL_TEXTURE_2D, material->lightMap->id);
        }

        // Como solo hay una difusa, es la misma que la base
        if (material->specularMap)
        {
            glActiveTexture(GL_TEXTURE0 + 2);
            shader.setInt("material.specularTexture", 2);
            shader.setBool("material.hasSpecularTexture", true);
            glBindTexture(GL_TEXTURE_2D, material->specularMap->id);
        }
        else
        {
            shader.setBool("material.hasSpecularTexture", false);
            shader.setVec4("material.specular", material->specular);
        }

        shader.setBool("material.IsReflective", material->IsReflective);
        shader.setFloat("material.shininess", material->shininess);
    }
}

void Mesh::addLightToRender(LightParams &light)
{
    lights.emplace_back(&light);
}

void Mesh::updateLights(ResourceShader &shader)
{
    int nPointLight{0}, nSpotLight{0};
    for (auto const &light : lights)
    {
        if (light->type == LightType::POINT_LIGHT && nPointLight < 4)
        {
            shader.setVec3("pointLights[" + std::to_string(nPointLight) + "].position", light->position);
            shader.setVec3("pointLights[" + std::to_string(nPointLight) + "].ambient", light->ambient);
            shader.setVec3("pointLights[" + std::to_string(nPointLight) + "].diffuse", light->diffuse);
            shader.setVec3("pointLights[" + std::to_string(nPointLight) + "].specular", light->specular);
            shader.setFloat("pointLights[" + std::to_string(nPointLight) + "].constant", light->constant);
            shader.setFloat("pointLights[" + std::to_string(nPointLight) + "].linear", light->linear);
            shader.setFloat("pointLights[" + std::to_string(nPointLight) + "].quadratic", light->quadratic);
            shader.setBool("pointLights[" + std::to_string(nPointLight) + "].HasAttenuation", light->HasAttenuation);
            nPointLight++;
        }
        else if (light->type == LightType::SPOT_LIGHT && nSpotLight < 4)
        {
            shader.setVec3("spotLights[" + std::to_string(nSpotLight) + "].position", light->position);
            shader.setVec3("spotLights[" + std::to_string(nSpotLight) + "].direction", light->direction);
            shader.setVec3("spotLights[" + std::to_string(nSpotLight) + "].ambient", light->ambient);
            shader.setVec3("spotLights[" + std::to_string(nSpotLight) + "].diffuse", light->diffuse);
            shader.setVec3("spotLights[" + std::to_string(nSpotLight) + "].specular", light->specular);
            shader.setFloat("spotLights[" + std::to_string(nSpotLight) + "].constant", light->constant);
            shader.setFloat("spotLights[" + std::to_string(nSpotLight) + "].linear", light->linear);
            shader.setFloat("spotLights[" + std::to_string(nSpotLight) + "].quadratic", light->quadratic);
            shader.setFloat("spotLights[" + std::to_string(nSpotLight) + "].cutOff", light->getCutOff());
            shader.setFloat("spotLights[" + std::to_string(nSpotLight) + "].outerCutOff", light->getOuterCutOff());
            shader.setBool("spotLights[" + std::to_string(nSpotLight) + "].HasAttenuation", light->HasAttenuation);
            nSpotLight++;
        }
        else
        {
            break;
        }
    }
    shader.setInt("sizePointsLights", nPointLight);
    shader.setInt("sizeSpotLights", nSpotLight);
    lights.clear();
}