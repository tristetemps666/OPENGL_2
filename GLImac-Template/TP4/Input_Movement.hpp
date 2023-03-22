#pragma once

#include <algorithm>
#include <iostream>
#include <array>
#include <cmath>
#include <utility>
#include <vector>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/glm.hpp>


// struture de la souris
struct Mouse{
    glm::dvec2 position {};
    glm::dvec2 delta {};
    double scroll{};

    int is_left_button_pressed;
};


struct MovementInput{
    int up_pressed;
    int down_pressed;
    int left_pressed;
    int right_pressed;

    int up_key;
    int down_key;
    int left_key;
    int right_key;


    void update_pressed_values(GLFWwindow *window){
        up_pressed = glfwGetKey(window, up_key);
        down_pressed = glfwGetKey(window, down_key);
        left_pressed = glfwGetKey(window, left_key);
        right_pressed = glfwGetKey(window, right_key);
    }

};
