#pragma once

#include <iostream>
#include <vector>
#include "glimac/Sphere.hpp"
#include "glimac/common.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <tiny_obj_loader.h>
#include <filesystem>
#include <glimac/glm.hpp>

void fill_vbo(GLuint& m_vbo, const glimac::Sphere& sphr)
{
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferData(GL_ARRAY_BUFFER, sphr.getVertexCount() * sizeof(glimac::ShapeVertex), sphr.getDataPointer(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void fill_vbo(GLuint& m_vbo, const std::vector<tinyobj::real_t> vertices)
{
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(tinyobj::real_t), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void setup_vao(GLuint& m_vao, const GLuint& m_vbo)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    const GLuint VERTEX_ATTR_POSITION  = 0;
    const GLuint NORMAL_ATTR_POSITION  = 1;
    const GLuint TEXTURE_ATTR_POSITION = 2;

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(NORMAL_ATTR_POSITION);
    glEnableVertexAttribArray(TEXTURE_ATTR_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, position));
    glVertexAttribPointer(NORMAL_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, normal));
    glVertexAttribPointer(TEXTURE_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, texCoords));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void setup_vao_from_obj(GLuint& m_vao, GLuint& m_ibo, const GLuint& m_vbo)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // we use the ibo because .obj works with indexed vertices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

    const GLuint VERTEX_ATTR_POSITION  = 0;
    const GLuint NORMAL_ATTR_POSITION  = 1;
    const GLuint TEXTURE_ATTR_POSITION = 2;

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(NORMAL_ATTR_POSITION);
    glEnableVertexAttribArray(TEXTURE_ATTR_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    const GLuint size_of_real_t = sizeof(tinyobj::real_t);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, size_of_real_t, (const GLvoid*)0);
    glVertexAttribPointer(NORMAL_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, size_of_real_t, (const GLvoid*)(3 * size_of_real_t));
    glVertexAttribPointer(TEXTURE_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, size_of_real_t, (const GLvoid*)(6 * size_of_real_t));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void fill_ibo(GLuint& m_ibo, const std::vector<tinyobj::index_t> indices)
{
    // get the array of the vertices (maybe there is an issue because there is an index array for each attribute)
    std::vector<int> vertices_index;
    for (auto const& indice : indices) {
        vertices_index.push_back(indice.vertex_index);
    }

    GLuint ibo;
    glGenBuffers(1, &ibo);
    m_ibo = ibo;

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices_index.size() * sizeof(int), vertices_index.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

class Mesh {
private:
    GLuint  m_vbo;
    GLuint  m_vao;
    GLuint  m_ibo = 999999999;
    GLsizei vertex_count;

public:
    Mesh()                 = default;
    Mesh(const Mesh& mesh) = default;

    ~Mesh()
    {
        glDeleteBuffers(1, get_vao_ptr());
        glDeleteVertexArrays(1, get_vbo_ptr());
    };

    // TODO constructor from a file path (.obj)
    Mesh(const glimac::Sphere& sphr)
    {
        fill_vbo(m_vbo, sphr);
        setup_vao(m_vao, m_vbo);
        vertex_count = sphr.getVertexCount();
    }
    Mesh(const std::string file_path);
    GLuint get_vao()
    {
        return m_vao;
    }
    GLuint get_vbo()
    {
        return m_vbo;
    }

    GLuint* get_vao_ptr()
    {
        return &m_vao;
    }
    GLuint* get_vbo_ptr()
    {
        return &m_vbo;
    }

    GLsizei get_vertex_count()
    {
        return vertex_count;
    }

    bool has_ibo()
    {
        return !(m_ibo == 999999999);
    }
    GLuint get_ibo()
    {
        return m_ibo;
    }
};

Mesh::Mesh(const std::string file_path)
{
    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_path.c_str());

    // is there any errors in the loading ?
    if (!warn.empty())
        std::cout << warn << std::endl;
    if (!err.empty())
        std::cout << err << std::endl;

    if (!ret) {
        exit(1);
    }

    fill_vbo(m_vbo, attrib.GetVertices());
    fill_ibo(m_ibo, shapes[0].mesh.indices);
    setup_vao_from_obj(m_vao, m_ibo, m_vbo);
    vertex_count = shapes[0].mesh.indices.size(); // mdr la gueule
}