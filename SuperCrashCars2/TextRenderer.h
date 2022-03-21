#pragma once

#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <map>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "texture.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "ShaderProgram.h"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // size of glyph
    glm::ivec2   Bearing;   // offset from baseline to left/top of glyph
    unsigned int Advance;   // horizontal offset to advance to next glyph
};


// A renderer class for rendering text displayed by a font loaded using the 
// FreeType library. A single font is loaded, processed into a list of Character
// items for later rendering.
class TextRenderer
{
public:
    // holds a list of pre-compiled Characters
    std::map<char, Character> Characters;

    // size of printed text
    float totalW, totalH;
    // shader used for text rendering
    ShaderProgram TextShader;
    TextRenderer(unsigned int width, unsigned int height);    
    ~TextRenderer() {};
    void Load(std::string font, unsigned int fontSize);
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);
private:
    // render state
    unsigned int VAO, VBO;
};

#endif 

