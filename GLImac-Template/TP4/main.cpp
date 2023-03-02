// #include <GL/glext.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <utility>
#include <vector>
#include "glimac/Image.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <glimac/glm.hpp>
#include <glimac/Sphere.hpp>

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
    std::cout << argc;
    //  argc;
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
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "TP4", nullptr, nullptr);
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



    glEnable(GL_DEPTH_TEST); 


    // setup the shaders
    glimac::FilePath       applicationPath(argv[0]);
    const glimac::FilePath vs_path = applicationPath.dirPath() + "TP4/shaders/3D.vs.glsl";

    const glimac::FilePath fs_path = applicationPath.dirPath() + "TP4/shaders/normal.fs.glsl";

    glimac::Program program = glimac::loadProgram(vs_path, fs_path);

    program.use();

    // get uniforms values
    GLint MVP_location = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
    GLint MV_location = glGetUniformLocation(program.getGLId(), "uMVMatrix");
    GLint Normal_location = glGetUniformLocation(program.getGLId(),"uNormalMatrix");




    // MATRIX
    glm::mat4 ProjMatrix;
    glm::mat4 MVMatrix;
    glm::mat4 NormalMatrix;
    glm::mat4 MVPMatrix;

    // calculate them
    MVMatrix = glm::translate(MVMatrix,glm::vec3(0,0,-5));
    MVPMatrix = ProjMatrix*MVMatrix;
    ProjMatrix = glm::perspective(glm::radians(70.f),
                                   aspectRatio
                                   ,0.1f,100.f);
    NormalMatrix =  glm::transpose(glm::inverse(MVMatrix)); // transforms tha affect normals


    ///// DATAS 
    glimac::Sphere sphr(1,16,32);


    // pass the vertex infos to the vbo
    GLuint vbo; 
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sphr.getVertexCount()*sizeof(glimac::ShapeVertex),sphr.getDataPointer(),GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,0);


    // setup the vao
    GLuint vao;
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint NORMAL_ATTR_POSITION = 1;
    const GLuint TEXTURE_ATTR_POSITION = 2;

    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, position));
    glVertexAttribPointer(NORMAL_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, normal));
    glVertexAttribPointer(TEXTURE_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, texCoords));


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // update matrix uniforms
        // send it to the shader
        glUniformMatrix4fv(MVP_location,1,false,glm::value_ptr(MVPMatrix));
        glUniformMatrix4fv(MV_location,1,false,glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(Normal_location,1,false,glm::value_ptr(NormalMatrix));

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES,0,sphr.getVertexCount());
        

        glClearColor(0.75f, 0.f, 0.75f, 1.f);
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}