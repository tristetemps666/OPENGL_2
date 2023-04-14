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

    // point light
    GLuint uIncidentLight;
    GLuint uLightColor;
    GLuint uLightIntensity;
    GLuint u_nb_light;

    // direction light
    GLuint uDirectionLight;
    GLuint uDirectionLightColor;
    GLuint uDirectionLightIntensity;
    GLuint u_direction_nb_light;

    BlinnPhongProgram(const glimac::FilePath& applicationPath)
        :

        m_Program(loadProgram(applicationPath.dirPath() + "TP4/shaders/3D.vs.glsl",
                              applicationPath.dirPath() + "TP4/shaders/light.fs.glsl"))
    {
        // Matrix
        uMVPMatrix    = glGetUniformLocation(m_Program.getGLId(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.getGLId(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.getGLId(), "uNormalMatrix");
        uTexture      = glGetUniformLocation(m_Program.getGLId(), "uTexture");

        // Material
        uDiffuse   = glGetUniformLocation(m_Program.getGLId(), "K_d");
        uGLossy    = glGetUniformLocation(m_Program.getGLId(), "K_s");
        uShininess = glGetUniformLocation(m_Program.getGLId(), "shininess");

        // Point light
        uIncidentLight  = glGetUniformLocation(m_Program.getGLId(), "w_i");
        uLightColor     = glGetUniformLocation(m_Program.getGLId(), "L_i");
        uLightIntensity = glGetUniformLocation(m_Program.getGLId(), "intensity");
        u_nb_light      = glGetUniformLocation(m_Program.getGLId(), "nb_light");

        // Directionnal light
        uDirectionLight          = glGetUniformLocation(m_Program.getGLId(), "direction_i");
        uDirectionLightColor     = glGetUniformLocation(m_Program.getGLId(), "L_i_direction");
        uDirectionLightIntensity = glGetUniformLocation(m_Program.getGLId(), "intensity_direction");
        u_direction_nb_light     = glGetUniformLocation(m_Program.getGLId(), "nb_light_directionnal");
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

    void passLight(const std::vector<PointLight>& list_light, const std::vector<DirectionalLight>& list_directionnal_light, const glm::mat4& viewMat)

    {
        std::vector<glm::vec3> list_pos_view;
        std::vector<glm::vec3> list_color;
        std::vector<float>     list_intensity;

        std::vector<glm::vec3> list_direction_view;
        std::vector<glm::vec3> list_color_directionnal;
        std::vector<float>     list_intensity_directionnal;

        // setup data list point light
        for (auto const& light : list_light) {
            // calculate incident vector in view coord
            glm::vec3 light_pos_view = transform_in_view_space(light.position, viewMat, 1);

            list_pos_view.push_back(light_pos_view);
            list_color.push_back(light.color);
            list_intensity.push_back(light.intensity);
        }
        int nb_light = list_light.size();

        glUniform3fv(uIncidentLight, nb_light, (const GLfloat*)list_pos_view.data());
        glUniform3fv(uLightColor, nb_light, (const GLfloat*)list_color.data());
        glUniform1fv(uLightIntensity, nb_light, (const GLfloat*)list_intensity.data());
        glUniform1i(u_nb_light, nb_light);

        // setup data list directional light
        for (auto const& dir_light : list_directionnal_light) {
            glm::vec3 light_dir_view = transform_in_view_space(dir_light.direction, viewMat, 0);
            list_direction_view.push_back(light_dir_view);
            list_color_directionnal.push_back(dir_light.color);
            list_intensity_directionnal.push_back(dir_light.intensity);
        }
        int nb_dir_light = list_directionnal_light.size();

        glUniform3fv(uDirectionLight, nb_dir_light, (const GLfloat*)list_direction_view.data());
        glUniform3fv(uDirectionLightColor, nb_dir_light, (const GLfloat*)list_color_directionnal.data());
        glUniform1fv(uDirectionLightIntensity, nb_dir_light, (const GLfloat*)list_intensity_directionnal.data());
        glUniform1i(u_direction_nb_light, nb_dir_light);
    }

    void passMaterial(Material& material)
    {
        glUniform3fv(uDiffuse, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(uGLossy, 1, glm::value_ptr(material.glossy));
        glUniform1f(uShininess, material.shininess);
        // glUniform1i(uTexture, 0);
    }

    glm::vec3 transform_in_view_space(glm::vec3 vector, glm::mat4 view_matrix, int is_point)
    {
        glm::vec4 h_vector{vector.x, vector.y, vector.z, is_point};
        glm::vec4 h_vector_view = view_matrix * h_vector;
        glm::vec3 vector_view{h_vector_view.x,
                              h_vector_view.y,
                              h_vector_view.z};
        return vector_view;
    }
};
