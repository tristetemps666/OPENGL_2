#pragma once

#include <iostream>
#include <cmath>
#include <utility>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/scalar_constants.hpp"
#include "glm/fwd.hpp"
#define GLFW_INCLUDE_NONE
#include <glimac/glm.hpp>

class FreeCamera {
    private :

        // polar coordinates
        glm::vec3 m_Position;
        float m_fPhi;
        float m_fTheta;

        glm::vec3 m_FrontVector;
        glm::vec3 m_LeftVector;
        glm::vec3 m_Upvector;

        void computeDirectionVectors();

    public :
        FreeCamera(){
            m_Position = glm::vec3(0.);
            m_fPhi = glm::pi<float>();
            m_fTheta = 0.;

            computeDirectionVectors();
        }
        ~FreeCamera() = default;


        void moveLeft(const float& t){
            m_Position += t*m_LeftVector;
        }
        
        void moveFront(const float& t){
            m_Position += t*m_FrontVector;
        }

        void rotateLeft(const float& degree){
            float rad = glm::radians(degree);
            m_fTheta += rad;
            computeDirectionVectors();
        }
        
        void rotateFront(const float& degree){
            float rad = glm::radians(degree);
            m_fPhi += rad;
            computeDirectionVectors();
        }

        glm::mat4 getViewMatrix() const{
            return glm::lookAt(m_Position,m_Position+m_FrontVector,m_Upvector);
        }



        friend std::ostream& operator<<(std::ostream& os, const FreeCamera& cam);


};


void FreeCamera::computeDirectionVectors(){
    m_FrontVector = glm::vec3(cosf(m_fTheta)*sinf(m_fPhi),
                              sinf(m_fTheta),
                              cosf(m_fTheta)*cosf(m_fPhi));

    m_LeftVector = glm::vec3(sinf(m_fPhi + glm::pi<float>()/2.),
                             0.,
                             cos(m_fPhi + glm::pi<float>()/2.));

    m_Upvector = glm::cross(m_FrontVector, m_LeftVector);

}

inline std::ostream& operator<<(std::ostream& os, const FreeCamera& cam)
{
    os << cam.m_Position << "/ phi : " << cam.m_fPhi << "/ theta : " << cam.m_fTheta;
    return os;
}