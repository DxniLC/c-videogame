#pragma once

#include <engines/resource/TResource.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>

#include <glm/vec2.hpp>

struct Character
{
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

struct ResourceFont : TResource
{
    explicit ResourceFont() = default;

    void loadFile(const char *path) override final;

    constexpr int getFontSize() const noexcept
    {
        return fontSize;
    }

    [[nodiscard]] Character getCharacterOnMap(char character) const noexcept;

private:
    const unsigned int maxCharacters{128};
    std::map<char, Character> m_Characters;
    int fontSize{48};
};