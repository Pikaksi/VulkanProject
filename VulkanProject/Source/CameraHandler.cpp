#include <vulkan/vulkan.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <chrono>
#include <iostream>
#include <algorithm>

#include "CameraHandler.hpp"
#include "PlayerInputHandler.hpp"



void CameraHandler::updateCameraTransform()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    float timePassed = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - timeLastFrame).count();
    timeLastFrame = currentTime;

    float speed = speedNormal 
        * (PlayerInputHandler::getInstance().shiftHeld ? slowerSpeedMultiplier : 1)
        * (PlayerInputHandler::getInstance().ctrlHeld ? fasterSpeedMultiplier : 1);

    if (PlayerInputHandler::getInstance().wHeld) {
        position += cameraForwardDirection() * speed * timePassed;
    }
    if (PlayerInputHandler::getInstance().sHeld) {
        position -= cameraForwardDirection() * speed * timePassed;
    }

    if (PlayerInputHandler::getInstance().dHeld) {
        position += cameraRightDirection() * speed * timePassed;
    }
    if (PlayerInputHandler::getInstance().aHeld) {
        position -= cameraRightDirection() * speed * timePassed;
    }

    if (PlayerInputHandler::getInstance().eHeld) {
        position += cameraUpDirection() * speed * timePassed;
    }
    if (PlayerInputHandler::getInstance().qHeld) {
        position -= cameraUpDirection() * speed * timePassed;
    }

    /*std::cout << std::setprecision(2) << std::fixed;
    std::cout << "coordinates are   " << position.x << "   " << position.y << "   " << position.z <<
        "  time passed is: " << timePassed <<
        "  rotation = " << rotationX << "   " << rotationY << "\n";*/

    rotationX += PlayerInputHandler::getInstance().mouseMovementX * sensitivity;
    rotationY += -PlayerInputHandler::getInstance().mouseMovementY * sensitivity;

    rotationY = std::clamp(rotationY, glm::radians(-89.0f), glm::radians(89.0f));

    // Has to be done because mouse movement is only updated if the mouse is moved.
    PlayerInputHandler::getInstance().mouseMovementX = 0.0f;
    PlayerInputHandler::getInstance().mouseMovementY = 0.0f;

}

void CameraHandler::getCameraMatrix(VkExtent2D swapChainExtent, CameraUniformBufferObject& ubo)
{
    // camera x rotation is handled in lookAt and y rotation in ubo.model
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    ubo.view = glm::lookAt(
        glm::vec3(position.x, position.y, position.z),
        glm::vec3(position.x, position.y, position.z) + cameraForwardDirection(),
        glm::vec3(0.0f, -1.0f, 0.0f) // Up is down and the screen is not flipped like usual to make the positive right axis appear on the right of the screen.
    );
    ubo.model = glm::mat4(1.0f); //glm::rotate(glm::mat4(1.0f), rotationY, glm::vec3(1.0f, 0.0f, 0.0f));

    ubo.proj = glm::perspective(glm::radians(85.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 500.0f);
    //ubo.proj[1][1] *= -1;
}

glm::vec3 CameraHandler::cameraRightDirection()
{
    return glm::rotate(glm::vec3(0.0f, 0.0f, 1.0f), rotationX + glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 CameraHandler::cameraUpDirection()
{
    glm::vec3 yFront = glm::rotate(glm::vec3(0.0f, 0.0f, 1.0f), rotationX, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 yFrontRot = glm::rotate(yFront, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    return glm::rotate(yFront, rotationY + glm::radians(90.0f), yFrontRot);
}

glm::vec3 CameraHandler::cameraForwardDirection()
{
    glm::vec3 frontXRotated = glm::rotate(glm::vec3(0.0f, 0.0f, 1.0f), rotationX, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 sideXRotated = glm::rotate(frontXRotated, glm::radians(90.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    return glm::rotate(frontXRotated, rotationY, sideXRotated);
}
