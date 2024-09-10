
#include "SkyBox.hpp"

#include <engines/graphic/resource/ResourceTexture.hpp>
#include <engines/graphic/resource/ResourceShader.hpp>

#include <GLAD/src/glad.h>

SkyBox::SkyBox(ResourceTexture *texture, ResourceShader *shader) : textures{texture}, shader{shader}
{
    setup();
}

SkyBox::~SkyBox()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &VBO);
}

void SkyBox::setup()
{
    // Create and link VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create and link Vertex Buffer Objects (VBO)
    glGenBuffers(1, &VBO);

    // Vertex
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs), &vertexs, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

void SkyBox::draw(glm::mat4 view)
{

    glDepthFunc(GL_LEQUAL); // Change Depth
    shader->use();
    // Remove translation on viewMatrix
    auto newView = glm::mat4(glm::mat3(view));
    shader->setMat4("view", newView);

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textures->id);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // Depth to Default
}