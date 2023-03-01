#include <glimac/glm.hpp>


inline glm::mat3x3 Rotate(const float &angle){
    return  glm::transpose(glm::mat3x3(cos(angle),sin(angle),0.,
                                      -sin(angle),cos(angle),0.,
                                       0.,        0.,        1.));

}


inline glm::mat3x3 Translate(const float &tx, const float &ty){
    
    return glm::transpose(glm::mat3x3(1.,0.,tx,
                                      0.,1.,ty,
                                      0.,0.,1.));

}


inline glm::mat3x3 Scale(const float &sx, const float &sy){
    return glm::mat3x3(sx,0.,0.,
                       0.,sy,0.,
                       0.,0.,1.);

}