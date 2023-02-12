#include <array>
#include <cmath>
#include <vector>
#include "glm/fwd.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <glimac/glm.hpp>

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
[[maybe_unused]] float aspectRatio = 1.f * window_width / window_height;

class Vertex2DColor {
public:
    [[maybe_unused]] glm::vec2 position;
    [[maybe_unused]] glm::vec3 color;

public:
    Vertex2DColor() = default;

    Vertex2DColor(glm::vec2 pos, glm::vec3 col)
        : position(pos), color(col) { position.x *= 1 / aspectRatio; };
};

std::vector<Vertex2DColor> Generate_circle_position(float radius, glm::vec2 center, unsigned int nb_points)
{
    std::vector<Vertex2DColor> list_point(nb_points);
    float                      delta_angle = 360.f / nb_points;

    for (unsigned int i = 0; i < nb_points; i++) {
        list_point[i].position = glm::vec2(cos(delta_angle * i), sin((delta_angle)*i)) + center;
    }

    return list_point;
}

int main(int argc, char* argv[])
{
    (void)argc;

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
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "TP1", nullptr, nullptr);
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

    // std::string triangle_fs = "C:/Users/debea/Documents/IMAC/IMAC_2/S4/PROG_GRAPHIQUE/OPENGL_2/GLImac-Template/TP1/shaders/triangle.fs.glsl";
    // std::string triangle_vs = "C:/Users/debea/Documents/IMAC/IMAC_2/S4/PROG_GRAPHIQUE/OPENGL_2/GLImac-Template/TP1/shaders/triangle.vs.glsl";

    // glimac::FilePath applicationPath(argv[0]);
    // glimac::Program  program = glimac::loadProgram(triangle_vs,
    //                                                triangle_fs);
    // program.use();

    glimac::FilePath applicationPath(argv[0]);
    glimac::Program  program = glimac::loadProgram(applicationPath.dirPath() + "TP1/shaders/triangle.vs.glsl",
                                                   applicationPath.dirPath() + "TP1/shaders/triangle.fs.glsl");
    program.use();

    // INITIALIZATION :

    GLuint vbo;
    GLuint vao;

    // VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // OLD PART WITHOUT STRUCT
    // x,y,r,g,b
    // GLfloat vertices[] = {-0.5f, -0.5f, 1.f, 0.f, 0.f,
    //                       0.5f, -0.5f, 0.f, 1.f, 0.f,
    //                       0.f, 0.5f, 0.f, 0.f, 1.f};

    // for (int i = 0; i < 2; i++) {
    //     vertices[5 * i] *= 1 / aspectRatio;
    // }

    // PART WITH STRUCT
    std::vector<Vertex2DColor> triangle_1 = {
        // first triangle
        Vertex2DColor(glm::vec2(-0.5, 0.5), glm::vec3(1, 0, 0)),
        Vertex2DColor(glm::vec2(0.5, 0.5), glm::vec3(0, 1, 0)),
        Vertex2DColor(glm::vec2(-0.5, -0.5), glm::vec3(0, 0, 1)),

        // second triangle
        Vertex2DColor(glm::vec2(0.5, 0.5), glm::vec3(0, 1, 0)),
        Vertex2DColor(glm::vec2(0.5, -0.5), glm::vec3(1, 1, 0)),
        Vertex2DColor(glm::vec2(-0.5, -0.5), glm::vec3(0, 0, 1)),
    };

    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(Vertex2DColor), triangle_1.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VAO
    const GLuint VERTEX_ATTR_POSITION = 3;
    const GLuint COLOR_ATTR_POSITION  = 8;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(COLOR_ATTR_POSITION);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)offsetof(Vertex2DColor, position));
    glVertexAttribPointer(COLOR_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)offsetof(Vertex2DColor, color));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.f, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

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