
#include "ParticleGenerator.hpp"

#include <GLAD/src/glad.h>

#include <iostream>

ParticleGenerator::ParticleGenerator(std::size_t amount, glm::vec2 sizeParticles, ResourceShader *shader, ResourceTexture *texture, int numSameTime)
    : sizeParticles{sizeParticles}, maxParticles{amount}, textureBase{texture}, shader{shader}, numSameTime{numSameTime}
{

    // ****** Particle Instance
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position and texture Coord attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);

    // ***** Buffer Transformation
    m_transformation.reserve(amount);

    glGenBuffers(1, &bufferData);
    glBindBuffer(GL_ARRAY_BUFFER, bufferData);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(amount * sizeof(float) * 5), nullptr, GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(sizeof(float) * 3));

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
}

void ParticleGenerator::deleteAllParticles()
{
    m_particles.clear();
    m_transformation.clear();
}

void ParticleGenerator::setShader(ResourceShader *customShader)
{
    shader = customShader;
}

void ParticleGenerator::addColor(glm::vec4 customColor)
{
    colorBase = customColor;
    MixColor = true;
}

void ParticleGenerator::addParticleLife(float lifeTime)
{
    particle_life = lifeTime;
}

ParticleGenerator::~ParticleGenerator()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glDeleteBuffers(1, &bufferData);

    m_effects.clear();
    m_particles.clear();
    m_transformation.clear();
}

void ParticleGenerator::generate()
{
    std::array<float, 5> position_size = {matrix[3].x, matrix[3].y, matrix[3].z, sizeParticles.x, sizeParticles.y};
    m_transformation.push_back(position_size);
    m_particles.emplace_back(matrix[3], sizeParticles, particle_life);
}

void ParticleGenerator::addEffect(std::unique_ptr<ParticleEffect> particleEffect)
{
    m_effects.emplace_back(std::move(particleEffect));
}

void ParticleGenerator::update()
{

    if (not IsRunning)
        return;

    elapsedTime += deltaTime;
    countTimer += deltaTime;

    std::size_t index{0};

    for (auto currentParticle = m_particles.begin(); currentParticle < m_particles.end();)
    {

        bool IsAlive = currentParticle->update(deltaTime);
        if (IsAlive)
        {
            for (auto &effect : m_effects)
                effect->applyEffect(&(*currentParticle), deltaTime);

            m_transformation[index][0] = currentParticle->position.x;
            m_transformation[index][1] = currentParticle->position.y;
            m_transformation[index][2] = currentParticle->position.z;
            m_transformation[index][3] = currentParticle->size.x;
            m_transformation[index][4] = currentParticle->size.y;

            currentParticle++;
            index++;
        }
        else
        {
            m_particles.erase(currentParticle);
            m_transformation.erase(m_transformation.begin() + int(index));
        }
    }

    if (elapsedTime <= Duration || IsLooping)
    {
        int size = int(m_particles.size()) + numSameTime;
        if (size <= int(maxParticles))
        {

            countTimer += deltaTime;

            if (1.0f / generate_4_second <= countTimer)
            {
                for (int i = 0; i < numSameTime; i++)
                {
                    generate();
                    countTimer = 0;
                }
            }
        }
    }

    updateBuffers();

    // std::cout << "\nParticules: " << m_particles.size();

    // Fallara en el skybox
    draw();
}
void ParticleGenerator::updateBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, bufferData);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(maxParticles * sizeof(float) * 5), &m_transformation[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleGenerator::draw()
{

    shader->use();

    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (textureBase)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureBase->id);
        shader->setVec4("color", colorBase);
        shader->setBool("MixColor", MixColor);
    }
    else
    {
        shader->setVec4("color", colorBase);
        shader->setBool("HasRect", true);
    }
    glBindVertexArray(VAO);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, GLsizei(m_transformation.size()));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    shader->setBool("HasRect", false);
}

void ParticleGenerator::setGenerationRatio(float time_seconds)
{
    generate_4_second = time_seconds;
}
