#include <algorithm>
#include <array>
#include <cmath>
#include <utility>
#include <vector>
#include "glimac/Image.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <glimac/glm.hpp>

#include "Vertex2Duv.hpp"
#include "Transform2D.hpp"

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


int main(int argc, char* argv[])
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


    glimac::FilePath       applicationPath(argv[0]);
    const glimac::FilePath vs_path = argc <= 1 ? applicationPath.dirPath() + "TP3/shaders/tex2D.vs.glsl"
                                               : applicationPath.dirPath() + "TP3/shaders/" + argv[1] + ".glsl";

    const glimac::FilePath fs_path = argc <= 1 ? applicationPath.dirPath() + "TP3/shaders/tex2D.fs.glsl"
                                               : applicationPath.dirPath() + "TP3/shaders/" + argv[2] + ".glsl";

    glimac::Program program = glimac::loadProgram(vs_path, fs_path);

    // pass the resolution to the shaders
    GLint res_loc = glad_glGetProgramResourceLocation(program.getGLId(), GL_UNIFORM, "iResolution");
    glad_glProgramUniform2f(program.getGLId(), res_loc, window_width, window_height);
    program.use();


    GLuint vbo;

    // setup vbo
    glGenBuffers(1,&vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

// 

    // THE SQUARE data
    std::vector<Vertex2Duv> square = {
        Vertex2Duv(glm::vec2(-0.5, -0.5), glm::vec2(0,1)), // bottom left
        Vertex2Duv(glm::vec2(0.5, -0.5), glm::vec2(1,1)),  // bottom right
        Vertex2Duv(glm::vec2(-0.5, 0.5), glm::vec2(0,0)),   // top left
        Vertex2Duv(glm::vec2(0.5, 0.5), glm::vec2(1,0))  // top right
    };
    

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
        0, 1, 2, 1, 3, 2
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
    glVertexAttribPointer(UV_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2Duv), (const GLvoid*)offsetof(Vertex2Duv, uv));

    glBindBuffer(GL_ARRAY_BUFFER, 0);



//////////////////
//////////////////


// load images
//////////////////
//////////////////
    std::cout << applicationPath.dirPath() << std::endl;
    std::unique_ptr<glimac::Image> img_ptr = glimac::loadImage("/home/6ima2/tristan.debeaune/Documents/prog_open_gl/OPENGL_2/assets/texture/triforce.png");
    if(img_ptr == nullptr) std::cout << "null";

    GLuint vto;
    glGenTextures(1,&vto);
    glBindTexture(GL_TEXTURE_2D, vto);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img_ptr.get()->getWidth(),img_ptr.get()->getHeight(),0,GL_RGBA,GL_FLOAT,img_ptr.get()->getPixels());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D,0);

    GLuint texLoc =glGetUniformLocation(program.getGLId(),
                                        "uTexture");


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.5f, 1.f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);


        // rotation along time
        glm::mat3  transform_mat = Rotate(0);
        // transform_mat *= Scale(sin(glfwGetTime()),sin(glfwGetTime()));
        // transform_mat = Translate(6*sin(glfwGetTime()),0.);

        GLuint location =glGetUniformLocation(program.getGLId(),
                                        "TransformMatrix");


        glUniformMatrix3fv(location, 1, GL_FALSE, 
                        glm::value_ptr(transform_mat));

        
        // sending aspect ratio;
        GLuint aspectRatio_location =glGetUniformLocation(program.getGLId(),"aspectRatio");
        glUniform1fv(aspectRatio_location,1,&aspectRatio);


        // glBindVertexArray(vao);
        // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // transform_mat*= Translate(0.2,0.2);
        // glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(transform_mat));

        // glBindVertexArray(vao);
        // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // transform_mat*= Translate(0.2,0.2);
        // glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(transform_mat));

        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, vto);
        glUniform1i(texLoc,0);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D,0);

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