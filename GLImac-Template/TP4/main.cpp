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

#include "random_sphere.hpp"
#include "shader_program.hpp"
#include "track_ball_camera.hpp"
#include "free_camera.hpp"
#include "Input_Movement.hpp"

int window_width  = 800;
int window_height = 600;


static Mouse mouse {};

static MovementInput keyboard = MovementInput{.up_key = GLFW_KEY_W,
                                              .down_key = GLFW_KEY_S,
                                              .left_key = GLFW_KEY_A,
                                              .right_key = GLFW_KEY_D};

TrackballCamera trackBallCamera = TrackballCamera(-5,0,0);
FreeCamera freeCam = FreeCamera();


void updateTrackBallCamera(TrackballCamera &cam, double delta = 0.);
void updateFreeCamera(FreeCamera &cam);

static void key_callback(GLFWwindow* /*window*/, int /*key*/, int /*scancode*/, int /*action*/, int /*mods*/)
{
}

void cursor_enter_callback(GLFWwindow *window,int entered){
    if(entered) std::cout << "dans la window";
    else std::cout << "hors de la window";
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
           
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_REPEAT){
            std::cout << "clic"<< std::endl;
            trackBallCamera.rotateLeft(mouse.delta.x);
            trackBallCamera.rotateUp(mouse.delta.y);

            std::cout << trackBallCamera << std::endl << std::endl;
        }

}

static void scroll_callback(GLFWwindow* /*window*/, double xoffset, double yoffset)
{
    mouse.scroll = yoffset;
    updateTrackBallCamera(trackBallCamera,yoffset);

    std::cout << mouse.scroll << std::endl;
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
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowSizeCallback(window, size_callback);
    glfwSetCursorEnterCallback(window, cursor_enter_callback);





    // setup the shaders
    glimac::FilePath       applicationPath(argv[0]);

    EarthProgram earthProgram(applicationPath);
    MoonProgram moonProgram(applicationPath);


    glimac::FilePath relative_path = applicationPath.dirPath()+".."+".."+"..";

    glEnable(GL_DEPTH_TEST);


    // MATRIX
    glm::mat4 ProjMatrix;
    glm::mat4 MVMatrix;
    glm::mat4 NormalMatrix;

    glm::mat4 MVPMatrix;

    // calculate them
    MVMatrix = glm::translate(glm::mat4(1),glm::vec3(0,0,-5));

    ProjMatrix = glm::perspective<float>(glm::radians(70.f),
                                   aspectRatio
                                   ,0.1f,100.f);

    MVPMatrix = ProjMatrix*MVMatrix;
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
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

    const GLuint VERTEX_ATTR_POSITION = 0;
    const GLuint NORMAL_ATTR_POSITION = 1;
    const GLuint TEXTURE_ATTR_POSITION = 2;

    glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
    glEnableVertexAttribArray(NORMAL_ATTR_POSITION);
    glEnableVertexAttribArray(TEXTURE_ATTR_POSITION);


    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, position));
    glVertexAttribPointer(NORMAL_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, normal));
    glVertexAttribPointer(TEXTURE_ATTR_POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, texCoords));

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    std::unique_ptr<glimac::Image> moon_ptr = glimac::loadImage(relative_path+"assets/texture/MoonMap.jpg");
    if(moon_ptr == nullptr) std::cout << "null";

    std::unique_ptr<glimac::Image> earth_ptr = glimac::loadImage(relative_path+"assets/texture/EarthMap.jpg");
    if(earth_ptr == nullptr) std::cout << "null";

    std::unique_ptr<glimac::Image> cloud_ptr = glimac::loadImage(relative_path+"assets/texture/CloudMap.jpg");
    if(earth_ptr == nullptr) std::cout << "null";


    // MOON    
    GLuint vto_moon;
    glGenTextures(1,&vto_moon);
    glBindTexture(GL_TEXTURE_2D, vto_moon);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,moon_ptr.get()->getWidth(),moon_ptr.get()->getHeight(),0,GL_RGBA,GL_FLOAT,moon_ptr.get()->getPixels());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D,0);


    // EARTH    

    GLuint vto_earth;
    glGenTextures(1,&vto_earth);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, vto_earth);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,earth_ptr.get()->getWidth(),earth_ptr.get()->getHeight(),0,GL_RGBA,GL_FLOAT,earth_ptr.get()->getPixels());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D,0);


    // CLOUD

    GLuint vto_cloud;
    glGenTextures(1,&vto_cloud);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, vto_cloud);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,cloud_ptr.get()->getWidth(),cloud_ptr.get()->getHeight(),0,GL_RGBA,GL_FLOAT,cloud_ptr.get()->getPixels());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D,0);

    [[maybe_unused]] float delta_time = 0.;


    auto list_axis = generate_spherical_vector(10,0.5);
    auto list_pos = generate_spherical_vector(10,2);


    glm::dvec2 mousePosStart;
    glm::dvec2 mousePosEnd;


    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        mouse.is_left_button_pressed = glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT);
        keyboard.update_pressed_values(window);
        
        std::cout << "Up : " << (char)keyboard.up_key << " / "  << keyboard.up_pressed << std::endl;
        std::cout << "down : " << (char)keyboard.down_key << " / " << keyboard.down_pressed << std::endl;
        std::cout << "left : " << (char)keyboard.left_key << " / " << keyboard.left_pressed << std::endl;
        std::cout << "right : " << (char)keyboard.right_key << " / " << keyboard.right_pressed << std::endl << std::endl;

        if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT)){
            updateTrackBallCamera(trackBallCamera);
        }


        glfwGetCursorPos(window, &mouse.position.x,&mouse.position.y);
        mousePosStart = mouse.position;


        float start_time = (float)glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.5f, 1.f, 0.5f, 1.f);

        // EARTH
        earthProgram.m_Program.use();
        glUniform1i(earthProgram.uEarthTexture, 0);
        glUniform1i(earthProgram.uCloudTexture, 1);

        updateFreeCamera(freeCam);
        // MVMatrix = trackBallCamera.getViewMatrix();
        MVMatrix = freeCam.getViewMatrix();
        MVPMatrix = ProjMatrix*MVMatrix;
        NormalMatrix =  glm::transpose(glm::inverse(MVMatrix)); // transforms tha affect normals


        glm::mat4 earthMVMatrix = glm::rotate(MVMatrix, start_time, glm::vec3(0, 1, 0));
        glUniformMatrix4fv(earthProgram.uMVMatrix, 1, GL_FALSE, 
	        glm::value_ptr(earthMVMatrix));
        glUniformMatrix4fv(earthProgram.uNormalMatrix, 1, GL_FALSE, 
	        glm::value_ptr(glm::transpose(glm::inverse(earthMVMatrix))));
        glUniformMatrix4fv(earthProgram.uMVPMatrix, 1, GL_FALSE, 
	        glm::value_ptr(ProjMatrix * earthMVMatrix));



        glBindVertexArray(vao);
                // set the cloud :
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, vto_cloud);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, vto_earth);

        glDrawArrays(GL_TRIANGLES,0,sphr.getVertexCount());
        glBindTexture(GL_TEXTURE_2D,0);



        // Moon 
        // set programme
        moonProgram.m_Program.use();

        // set the moon texture on the unit 1 and bind it
        glUniform1i(moonProgram.uTexture, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, vto_moon);

        for(unsigned int i=0; i< list_axis.size()-1; i++){
            auto axis = list_axis[i];
            auto start_pos = list_pos[i];

            glm::mat4 mat = get_transformations(start_time,MVMatrix,axis,start_pos);

            glUniformMatrix4fv(moonProgram.uMVMatrix, 1, GL_FALSE, 
                glm::value_ptr(mat));
            glUniformMatrix4fv(moonProgram.uNormalMatrix, 1, GL_FALSE, 
                glm::value_ptr(glm::transpose(glm::inverse(mat))));
            glUniformMatrix4fv(moonProgram.uMVPMatrix, 1, GL_FALSE, 
                glm::value_ptr(ProjMatrix * mat));
            
            glBindVertexArray(vao);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, vto_moon);

            glDrawArrays(GL_TRIANGLES,0,sphr.getVertexCount());
            glBindTexture(GL_TEXTURE_2D,0);

        }


        glBindVertexArray(0);


        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();

        // GET THE DELTAS
        float end_time = glfwGetTime();
        delta_time = end_time-start_time;

        glfwGetCursorPos(window, &mousePosEnd.x,&mousePosEnd.y);

        mouse.delta = mousePosEnd-mousePosStart;

        
    }
    glDeleteBuffers(1,&vbo);
    glDeleteVertexArrays(1,&vao);
    glfwTerminate();
    return 0;
}


void updateTrackBallCamera(TrackballCamera &cam, double delta){
    cam.rotateLeft(mouse.delta.x/100);
    cam.rotateUp(mouse.delta.y/100);
    cam.moveFront(delta);
}

void updateFreeCamera(FreeCamera &cam){ // only the mouse for now
    if (mouse.is_left_button_pressed == 1){
        cam.rotateLeft(-mouse.delta.y/4.);
        cam.rotateFront(-mouse.delta.x/4.);
    }

    cam.moveFront((keyboard.up_pressed-keyboard.down_pressed)/10.);
    cam.moveLeft((keyboard.left_pressed-keyboard.right_pressed)/10.);
}