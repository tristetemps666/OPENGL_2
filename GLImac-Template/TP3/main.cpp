#include <algorithm>
#include <array>
#include <cmath>
#include <utility>
#include <vector>
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <glimac/glm.hpp>

#include "Vertex2Duv.hpp"

int window_width  = 1280;
int window_height = 720;

static void key_callback(GLFWwindow* /*window*/, int /*key*/, int /*scancode*/, int /*action*/, int /*mods*/)
{
}

static void mouse_button_callback(GLFWwindow* /*window*/, int /*button*/, int /*action*/, int /*mods*/)
{
}

static void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double /*yoffset*/)
{
}

static void cursor_position_callback(GLFWwindow* /*window*/, double /*xpos*/, double /*ypos*/)
{
}

static void size_callback(GLFWwindow* /*window*/, int width, int height)
{
    window_width  = width;
    window_height = height;
}

static float aspectRatio = 1.f * window_width / window_height;


int main()
{
    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    /* Create a window and its OpenGL context */
#ifdef __APPLE__
    /* We need to explicitly ask for a 3.3 context on Mac */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "TP3", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Intialize glad (loads the OpenGL functions) */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    /* Hook input callbacks */
    glfwSetKeyCallback(window, &key_callback);
    glfwSetMouseButtonCallback(window, &mouse_button_callback);
    glfwSetScrollCallback(window, &scroll_callback);
    glfwSetCursorPosCallback(window, &cursor_position_callback);
    glfwSetWindowSizeCallback(window, &size_callback);



    GLuint vbo;

    // setup vbo
    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);


    // THE SQUARE data
    std::vector<Vertex2Duv> square = {
        Vertex2Duv(glm::vec2(-0.5, -0.5), glm::vec2()), // top left
        Vertex2Duv(glm::vec2(0.5, -0.5), glm::vec2()),  // top right
        Vertex2Duv(glm::vec2(0.5, 0.5), glm::vec2()),   // bottom left
        Vertex2Duv(glm::vec2(-0.5, 0.5), glm::vec2()),  // bottom right
    };
    std::for_each(square.begin(), square.end(), [&](Vertex2Duv &vertex){
        vertex.position.x*= 1/aspectRatio;
    });


// vbo et ibo
//////////////////
//////////////////
    // add the square to the buffer
    glBufferData(GL_ARRAY_BUFFER, square.size() * sizeof(Vertex2Duv), square.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    // setup the ibo
    GLuint ibo;
    glGenBuffers(1,&ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    
    std::vector<glm::uint32_t> indices = {
        0, 1, 2, 0, 2, 3
    };

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
//////////////////
//////////////////


// vao
//////////////////
//////////////////
    GLuint vao;
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint UV_ATTR_POSITION = 1;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(UV_ATTR_POSITION);
    


    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2Duv), (const GLvoid*)offsetof(Vertex2Duv, position));
    glVertexAttribPointer(UV_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2Duv), (const GLvoid*)offsetof(Vertex2Duv, uv));

    glBindBuffer(GL_ARRAY_BUFFER, 0);



//////////////////
//////////////////






    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.5f, 1.f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glfwTerminate();
    return 0;
}