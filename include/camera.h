#pragma once

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    UP,
    DOWN,
    LEFT,
    RIGHT
};


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Camera Attributes
    glm::vec3 Target;
    GLfloat Radius;
    GLfloat Yaw;
    GLfloat Pitch;
    glm::vec3 WorldUp;

    glm::vec3 Position;
    // Camera options
    GLfloat Aspect;

    // Constructor with vectors
    Camera(glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f), GLfloat radius = 40.0f, GLfloat yaw = 90.0f, GLfloat pitch = 0.0f) : WorldUp(glm::vec3(0.0f,1.0f,0.0f)),Aspect(35.0f)
    {
        this->Target = target;
        this->Radius = radius;
        this->Yaw = yaw;
        this->Pitch = pitch;
        this->updateCameraVectors();
    }
    
    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->Position, this->Target, this->WorldUp);
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void Move(Camera_Movement direction, GLfloat deltaTime, GLboolean constrainPitch = true)
    {
        GLfloat offset = 30.0f * deltaTime;

        if(direction == UP) this->Pitch += offset;
        if(direction == DOWN) this->Pitch -= offset;
        if(direction == LEFT) this->Yaw += offset;
        if(direction == RIGHT) this->Yaw -= offset;

        if(constrainPitch)
        {
            if(this->Pitch > 89.0f) this->Pitch = 89.0f;
            if(this->Pitch < -89.0f) this->Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Eular angles
        this->updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void Zoom(GLfloat yoffset)
    {
        //if(this->Aspect >= 1.0f && this->Aspect <= 45.0f)
        this->Aspect += yoffset;
        if(this->Aspect < 1.0f) this->Aspect = 1.0f;
        if(this->Aspect > 45.0f) this->Aspect = 45.0f;
    }

private:
    // Calculates the front vector from the Camera's (updated) Eular Angles
    void updateCameraVectors()
    {
        glm::vec3 position;
        position.x = this->Target.x + this->Radius * cos(glm::radians(this->Pitch)) * cos(glm::radians(this->Yaw));
        position.y = this->Target.y + this->Radius * sin(glm::radians(this->Pitch));
        position.z = this->Target.z + this->Radius * cos(glm::radians(this->Pitch)) * sin(glm::radians(this->Yaw));
        this->Position = position;
    }
};

