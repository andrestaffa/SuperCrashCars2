#pragma once

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <stb_image.h>
#include <vector>
#include <string>
#include <iostream>

#include "ShaderProgram.h"

class Image {
public:
    Image(float Width, float Height) : shader("shaders/image.vert", "shaders/image.frag") {
        // configure VAO/VBO
        glDeleteVertexArrays(1, &quadVAO);
        unsigned int VBO;
        float vertices[] = {
            // pos      // tex
            0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 1.0f, 1.0f,

            0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(quadVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Width),
            static_cast<float>(Height), 0.0f, -1.0f, 1.0f);
        shader.use();
        shader.setInt("image", 0);
        shader.setMat4("projection", projection);
    }

    void draw(Texture& texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color) {
        // prepare transformations
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)

        model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
        model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
        model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

        model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

        shader.setMat4("model", model);
        shader.setVector3("spriteColor", color);

        glActiveTexture(GL_TEXTURE0);
        texture.bind();
        glBindVertexArray(quadVAO);
        //glEnable(GL_FRAMEBUFFER_SRGB);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
private:
    // Render state
    ShaderProgram shader;
    unsigned int quadVAO;
};
