
#include "ResourceFont.hpp"

#include <iostream>

#include <GLAD/src/glad.h>

void ResourceFont::loadFile(const char *path)
{
    FT_Library m_libraryFont;

    if (FT_Init_FreeType(&m_libraryFont))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;

    if (FT_New_Face(m_libraryFont, path, 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, FT_UInt(fontSize));

    // Solo queremos representar un byte para el color. Un glifo esta en escala de grises y representado por un byte de color  y queremos crear una textura donde cada byte representa el rojo, primer byte del vector
    // Como queremos usar un byte de color hay que tener cuidado ya que openGL requiere de que las texturas tengan una alineacion de 4 bytes, su tamaño es multiplo de 4
    // Si quitamos ese problema de alineacion para que sea 1 byte, no tendremos problemas para el ancho de la textura ya que puede ser el que sea
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    // Ahora evitaremos que hayan problemas de alineacion ya que establecemos solo un byte por pixel

    for (unsigned char c = 0; c < maxCharacters; c++)
    {
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // Generate Texture
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            GLsizei(face->glyph->bitmap.width),
            GLsizei(face->glyph->bitmap.rows),
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer);

        // Texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Save Character
        Character character = {
            textureID,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (unsigned int)face->glyph->advance.x,
        };

        m_Characters.emplace(c, character);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(m_libraryFont);
}

Character ResourceFont::getCharacterOnMap(char character) const noexcept
{
    auto itr = m_Characters.find(character);
    if (itr != m_Characters.end())
        return itr->second;
    return m_Characters.begin()->second;
}