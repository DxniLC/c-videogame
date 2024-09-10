
#include "ResourceShader.hpp"

#include <GLAD/src/glad.h>

#include <iostream>
#include <fstream>

#include <sstream>

ResourceShader::ResourceShader(std::string vertPathString, std::string fragPathString)
{

    filename = vertPathString + " ---- " + fragPathString;

    // INITIAL VAR TO ERRORS
    int success;
    char infoLog[512];

    // SHADERS programID AND FILENAMES
    unsigned int vShaderId, fShaderId;
    const char *vShaderCode, *fShaderCode;

    // CREATE SHADERS
    vShaderId = glCreateShader(GL_VERTEX_SHADER);
    fShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // READ FILES
    std::string vertCode = readFile(vertPathString.c_str());
    std::string fragCode = readFile(fragPathString.c_str());


    vShaderCode = vertCode.c_str();
    fShaderCode = fragCode.c_str();

    // DEFINE FILES TO SHADERSOURCE
    glShaderSource(vShaderId, 1, (const char **)&vShaderCode, NULL);
    glShaderSource(fShaderId, 1, &fShaderCode, NULL);

    // COMPILE SHADERS
    glCompileShader(vShaderId);
    glCompileShader(fShaderId);

    // CHECK IF SHADERS HAVE ERRORS
    glGetShaderiv(vShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vShaderId, 512, NULL, infoLog);
        std::cout << "\nERROR::ResourceShader::VERTEX::COMPILATION_FAILED\n\n"
                  << infoLog << std::endl;
    }
    glGetShaderiv(fShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fShaderId, 512, NULL, infoLog);
        std::cout << "\nERROR::ResourceShader::FRAGMENT::COMPILATION_FAILED\n\n"
                  << infoLog << std::endl;
    }

    // CREATE PROGRAM AND LINK SHADERS
    programID = glCreateProgram();
    glAttachShader(programID, vShaderId);
    glAttachShader(programID, fShaderId);

    glLinkProgram(programID);

    // CHECK IF PROGRAM HAVE ERRORS
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programID, 512, NULL, infoLog);
        std::cout << "\nERROR::PROGRAM::LINK::PROGRAM_LINK_FAILED\n\n"
                  << infoLog << std::endl;
    }

    // DELETE SHADERS
    glDeleteShader(vShaderId);
    glDeleteShader(fShaderId);
}

ResourceShader::~ResourceShader()
{
    drop();
}

constexpr unsigned int ResourceShader::getID() const noexcept
{
    return programID;
}

void ResourceShader::drop()
{
    std::cout << "\nDROP PROGRAM\n";
    glDeleteProgram(programID);
}

void ResourceShader::use()
{
    glUseProgram(programID);
}

void ResourceShader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}
void ResourceShader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}
void ResourceShader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void ResourceShader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void ResourceShader::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
}
void ResourceShader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void ResourceShader::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}
void ResourceShader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void ResourceShader::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
}
void ResourceShader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void ResourceShader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void ResourceShader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

std::string ResourceShader::readFile(const char *filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "\n*** ERROR: failed to open file: " << filename << "\n\n";
        return nullptr;
    }

    std::stringstream buffer;

    buffer << file.rdbuf();

    std::string content = buffer.str();

    file.close();

    return content;
}