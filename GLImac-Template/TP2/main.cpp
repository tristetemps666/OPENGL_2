//#include <corecrt.h>
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
 float aspectRatio = 1.f * window_width / window_height;

class Vertex2DColor {
public:
     glm::vec2 position;
     glm::vec3 color;

public:
    Vertex2DColor() = default;

    Vertex2DColor(glm::vec2 pos, glm::vec3 col)
        : position(pos), color(col) { position.x *= 1 / aspectRatio; };
};

std::vector<Vertex2DColor> Generate_circle_position(float radius, glm::vec2 center, unsigned int nb_points)
{
    std::vector<Vertex2DColor> list_vertices;
    float                      delta_angle = 2 * glm::pi<float>() / nb_points;

    auto get_pos_and_Color = [delta_angle](glm::vec2 center, float radius, unsigned int i) {
        glm::vec2 pos_i = radius * glm::vec2(cos(delta_angle * i), sin((delta_angle)*i)) + center;
        glm::vec3 col_i = glm::vec3(pos_i, 1.f);
        return Vertex2DColor(pos_i, col_i);
    };

    list_vertices.push_back(Vertex2DColor(center, glm::vec3(1, 1, 1)));
    for (unsigned int i = 0; i < nb_points; i++)
        list_vertices.push_back(get_pos_and_Color(center, radius, i));

    return list_vertices;
}

std::vector<glm::uint32_t> Generate_circle_indices(unsigned int nb_points)
{
    std::vector<glm::uint32_t> list_indices;
    for (unsigned int i = 0; i < nb_points - 1; i++) {
        list_indices.push_back(i + 1);
        list_indices.push_back(0);
        list_indices.push_back((i + 2));
    }

    list_indices.push_back(nb_points);
    list_indices.push_back(0);
    list_indices.push_back(1);

    return list_indices;
}

std::pair<std::vector<Vertex2DColor>, std::vector<glm::uint32_t>> Generate_circle_data(float radius, glm::vec2 center, unsigned int nb_points)
{
    return std::pair<std::vector<Vertex2DColor>, std::vector<glm::uint32_t>>(
        Generate_circle_position(radius, center, nb_points),
        Generate_circle_indices(nb_points));
}

int main(int argc, char* argv[])
{
    //(void)argc;

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
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "TP2", nullptr, nullptr);
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
    const glimac::FilePath vs_path = argc <= 1 ? applicationPath.dirPath() + "TP2/shaders/color2D.vs.glsl"
                                               : applicationPath.dirPath() + "TP2/shaders/" + argv[1] + ".glsl";

    const glimac::FilePath fs_path = argc <= 1 ? applicationPath.dirPath() + "TP2/shaders/color2D.fs.glsl"
                                               : applicationPath.dirPath() + "TP2/shaders/" + argv[2] + ".glsl";

    glimac::Program program = glimac::loadProgram(vs_path, fs_path);

    // pass the resolution to the shaders
    GLint res_loc = glad_glGetProgramResourceLocation(program.getGLId(), GL_UNIFORM, "iResolution");
    glad_glProgramUniform2f(program.getGLId(), res_loc, window_width, window_height);
    program.use();

    // INITIALIZATION :

    GLuint vbo;
    GLuint vao;

    // VBO
    glGenBuffers(1, &vbo);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo); // old part without buffer index
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // PART WITH STRUCT
    std::vector<Vertex2DColor> square = {
        // first triangle
        Vertex2DColor(glm::vec2(-0.5, -0.5), glm::vec3(1, 0, 0)), // top left
        Vertex2DColor(glm::vec2(0.5, -0.5), glm::vec3(0, 1, 0)),  // top right
        Vertex2DColor(glm::vec2(0.5, 0.5), glm::vec3(0, 0, 1)),   // bottom left
        Vertex2DColor(glm::vec2(-0.5, 0.5), glm::vec3(1, 1, 0)),  // bottom right
    };

    auto circle_data = Generate_circle_data(1, glm::vec2(0, 0), 100);

    // auto circle = Generate_circle_position(1, glm::vec2(0, 0), 100); // old version

    // SQUARE
    glBufferData(GL_ARRAY_BUFFER, square.size() * sizeof(Vertex2DColor), square.data(), GL_STATIC_DRAW);

    // CIRCLE
    // glBufferData(GL_ARRAY_BUFFER, circle_data.first.size() * sizeof(Vertex2DColor), circle_data.first.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // IBO
    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    std::vector<glm::uint32_t> indices = {
        0, 1, 2, 0, 2, 3};

    // SQUARE
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * indices.size(), indices.data(), GL_STATIC_DRAW);

    // CIRCLE
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circle_data.second) * circle_data.second.size(), circle_data.second.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // VAO
    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint COLOR_ATTR_POSITION  = 1;
    const GLuint MOUSE_ATTR_POSITION  = 2;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(COLOR_ATTR_POSITION);
    glEnableVertexAttribArray(MOUSE_ATTR_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(VERTEX_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)offsetof(Vertex2DColor, position));
    glVertexAttribPointer(COLOR_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2DColor), (const GLvoid*)offsetof(Vertex2DColor, color));
    glVertexAttribPointer(MOUSE_ATTR_POSITION, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), (const GLvoid*)(2 * sizeof(double)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glm::dvec2 mousePos;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
        std::cout << mousePos << std::endl;

        glClearColor(1.f, 0.5f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);


        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);

        // CIRCLE
        // glDrawElements(GL_TRIANGLES, circle_data.second.size(), GL_UNSIGNED_INT, 0);

        // SQUARE
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