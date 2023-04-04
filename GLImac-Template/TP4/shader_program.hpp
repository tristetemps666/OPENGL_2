#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <utility>
#include <vector>
#include "glimac/Image.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/random.hpp"
#include "light.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glimac/FilePath.hpp>
#include <glimac/Program.hpp>
#include <glimac/Sphere.hpp>
#include <glimac/glm.hpp>
#include "Material.hpp"
#include "light.hpp"

struct EarthProgram {
    glimac::Program m_Program;

    // the uniforms
    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uEarthTexture;
    GLint uCloudTexture;

    EarthProgram(const glimac::FilePath& applicationPath)
        : m_Program(loadProgram(applicationPath.dirPath() + "TP4/shaders/3D.vs.glsl",
                                applicationPath.dirPath() + "TP4/shaders/multiTex3D.fs.glsl"))
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uEarthTexture = glGetUniformLocation(m_Program.getGLId(), "uEarthTexture");
        uCloudTexture = glGetUniformLocation(m_Program.getGLId(), "uCloudTexture");
    }
};

struct MoonProgram {
    glimac::Program m_Program;

    // 3D
    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;

    MoonProgram(const glimac::FilePath& applicationPath)
        :

        m_Program(loadProgram(applicationPath.dirPath() + "TP4/shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "TP4/shaders/light.fs.glsl"))
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uTexture      = glGetUniformLocation(m_Program.getGLId(), "uTexture");
    }
};

struct BlinnPhongProgram { // With point light
    glimac::Program m_Program;

    // 3D
    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;

    // material
    GLuint uDiffuse;
    GLuint uGLossy;
    GLuint uShininess;

    // light
    GLuint uIncidentLight;
    GLuint uLightColor;

    BlinnPhongProgram(const glimac::FilePath& applicationPath)
        :

        m_Program(loadProgram(applicationPath.dirPath() + "TP4/shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "TP4/shaders/light.fs.glsl"))
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uTexture      = glGetUniformLocation(m_Program.getGLId(), "uTexture");

        uDiffuse   = glGetUniformLocation(m_Program.getGLId(), "K_d");
        uGLossy    = glGetUniformLocation(m_Program.getGLId(), "K_s");
        uShininess = glGetUniformLocation(m_Program.getGLId(), "shininess");

        uIncidentLight = glGetUniformLocation(m_Program.getGLId(), "w_i");
        uLightColor    = glGetUniformLocation(m_Program.getGLId(), "L_i");
    }

    void passMatrix(const glm::mat4& MV, const glm::mat4& ProjMatrix)
    {
        glUniformMatrix4fv(uMVMatrix, 1, GL_FALSE,
                           glm::value_ptr(MV));
        glUniformMatrix4fv(uNormalMatrix, 1, GL_FALSE,
                           glm::value_ptr(glm::transpose(glm::inverse(MV))));
        glUniformMatrix4fv(uMVPMatrix, 1, GL_FALSE,
                           glm::value_ptr(ProjMatrix * MV));
    }

    void passLight(const PointLight& light, const glm::mat4& viewMat)
    {
        // calculate incident vector in view coord

        glm::vec4 h_light_pos{light.position.x, light.position.y, light.position.z, 1};
        glm::vec4 h_light_pos_view = viewMat * h_light_pos;
        glm::vec3 light_pos_view{h_light_pos_view.x,
                                 h_light_pos_view.y,
                                 h_light_pos_view.z};

        glUniform3fv(uIncidentLight, 1, glm::value_ptr(light_pos_view));
        glUniform3fv(uLightColor, 1, glm::value_ptr(light.color));
        std::cout << light_pos_view << std::endl
                  << std::endl
                  << std::endl;
    }

    void passMaterial(Material& material)
    {
        glUniform3fv(uDiffuse, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(uGLossy, 1, glm::value_ptr(material.glossy));
        glUniform1f(uShininess, material.shininess);
        // glUniform1i(uTexture, 0);
    }
};
