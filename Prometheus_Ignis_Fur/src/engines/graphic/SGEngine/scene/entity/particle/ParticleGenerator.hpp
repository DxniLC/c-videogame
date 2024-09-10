#pragma once

#include "../TEntity.hpp"

#include "Particle.hpp"

#include "ParticleEffect.hpp"

#include <vector>
#include <array>

#include <memory>

#include <glm/glm.hpp>

#include <engines/graphic/resource/ResourceShader.hpp>
#include <engines/graphic/resource/ResourceTexture.hpp>

struct ParticleGenerator : TEntity
{
    explicit ParticleGenerator(std::size_t amount, glm::vec2 sizeParticles, ResourceShader *shader, ResourceTexture *texture = nullptr, int numSameTime=1);

    ~ParticleGenerator();

    void setShader(ResourceShader *customShader);

    void addColor(glm::vec4 customColor);

    void addParticleLife(float lifeTime);

    void setGenerationRatio(float time_seconds);

    void generate();

    void update() override final;

    void addEffect(std::unique_ptr<ParticleEffect> particleEffect);

    void deleteAllParticles();

    void draw();

    bool MixColor{false};

    glm::vec2 sizeParticles{0.2f};

    float Duration{50.0f};

    bool IsLooping{false};
    bool IsRunning{true};

private:
    std::size_t maxParticles{0};

    ResourceTexture *textureBase{nullptr};

    glm::vec4 colorBase{1.0f};

    ResourceShader *shader{nullptr};

    int numSameTime{1};

    float elapsedTime{0.0};
    
    float particle_life{10.0f};

    float generate_4_second{1.0f};
    float countTimer{0.0f};


    std::vector<Particle> m_particles;
    std::vector<std::unique_ptr<ParticleEffect>> m_effects;

    unsigned int bufferData;
    std::vector<std::array<float, 5>> m_transformation; // vec3 position __ vec2 size

    unsigned int VAO, VBO, EBO;

    float vertices[8] = {
        // position     // texture coords
        1.0f, 1.0f, // top right
        1.0f, 0.0f, // bottom right
        0.0f, 0.0f, // bottom left
        0.0f, 1.0f, // top left
    };

    unsigned int indices[6] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    void updateBuffers();
};