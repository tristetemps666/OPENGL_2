#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <utility>
#include "Mesh.hpp"
#include "glimac/Sphere.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/glm.hpp>

void fill_vbo(GLuint& vbo, const glimac::Sphere& sphr)
{
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sphr.getVertexCount() * sizeof(glimac::ShapeVertex), sphr.getDataPointer(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void setup_vao(GLuint& vao, const GLuint& vbo)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const GLuint VERTEX_ATTR_POSITION  = 0;
    const GLuint NORMAL_ATTR_POSITION  = 1;
    const GLuint TEXTURE_ATTR_POSITION = 2;

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(NORMAL_ATTR_POSITION);
    glEnableVertexAttribArray(TEXTURE_ATTR_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, position));
    glVertexAttribPointer(NORMAL_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, normal));
    glVertexAttribPointer(TEXTURE_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, texCoords));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

class Mesh {
private:
    GLuint vbo;
    GLuint vao;

public:
    // TODO constructor from a file path (.obj)
    Mesh(const glimac::Sphere& sphr)
    {
        fill_vbo(vbo, sphr);
        setup_vao(vao, vbo);
    }
};
