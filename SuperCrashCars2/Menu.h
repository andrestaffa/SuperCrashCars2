#pragma once

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <stb_image.h>
#include <vector>
#include <string>
#include <iostream>

#include "ShaderProgram.h"
#include "Texture.h"

class Menu {
public:
    ShaderProgram shader;
    CPU_Geometry cpuGeom;
    GPU_Geometry gpuGeom;

    Menu() : shader("shaders/menu.vert", "shaders/menu.frag") {

        //SINGLE PLAYER BUTTON
        cpuGeom.verts.push_back(glm::vec3(-0.3f, 0.9f, 0.f)); //topleft
        cpuGeom.verts.push_back(glm::vec3(0.3f, 0.9f, 0.f)); //topright
        cpuGeom.verts.push_back(glm::vec3(0.3f, 0.4f, 0.f)); //bottomright
        cpuGeom.verts.push_back(glm::vec3(0.3f, 0.4f, 0.f)); //bottomright
        cpuGeom.verts.push_back(glm::vec3(-0.3f, 0.4f, 0.f)); //bottomleft
        cpuGeom.verts.push_back(glm::vec3(-0.3f, 0.9f, 0.f)); //topleft

        cpuGeom.texCoords.push_back(glm::vec2(0.f, 1.f)); //topleft
        cpuGeom.texCoords.push_back(glm::vec2(1.f, 1.f)); //topright
        cpuGeom.texCoords.push_back(glm::vec2(1.f, 0.f)); //bottomright
        cpuGeom.texCoords.push_back(glm::vec2(1.f, 0.f)); //bottomright
        cpuGeom.texCoords.push_back(glm::vec2(0.f, 0.f)); //bottomleft
        cpuGeom.texCoords.push_back(glm::vec2(0.f, 1.f)); //topleft

        gpuGeom.setVerts(cpuGeom.verts);
        gpuGeom.setTexCoords(cpuGeom.texCoords);
    }

    void draw() {
        shader.use();
        Texture texture("menu/singleplayer/singleplayer.png", GL_LINEAR);
        texture.bind();
        gpuGeom.bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        texture.unbind();
    }
};
