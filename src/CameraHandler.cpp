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
#include <glm/geometric.hpp>

#include "CameraHandler.hpp"
#include "PlayerInputHandler.hpp"

void CameraHandler::updateCameraTransform()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    float timePassed = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - timeLastFrame).count();
    timeLastFrame = currentTime;

    float speed = speedNormal * (inputHandler.keyHeld(GLFW_KEY_LEFT_SHIFT) ? slowerSpeedMultiplier : 1) *
                  (inputHandler.keyHeld(GLFW_KEY_LEFT_ALT) ? fasterSpeedMultiplier : 1);

    if (inputHandler.keyHeld(GLFW_KEY_W)) {
        position += cameraForwardDirection() * speed * timePassed;
    }
    if (inputHandler.keyHeld(GLFW_KEY_S)) {
        position -= cameraForwardDirection() * speed * timePassed;
    }

    if (inputHandler.keyHeld(GLFW_KEY_D)) {
        position += cameraRightDirection() * speed * timePassed;
    }
    if (inputHandler.keyHeld(GLFW_KEY_A)) {
        position -= cameraRightDirection() * speed * timePassed;
    }

    if (inputHandler.keyHeld(GLFW_KEY_SPACE)) {
        position += cameraUpDirection() * speed * timePassed;
    }
    if (inputHandler.keyHeld(GLFW_KEY_LEFT_CONTROL)) {
        position -= cameraUpDirection() * speed * timePassed;
    }

    /*std::cout << std::setprecision(2) << std::fixed;
    std::cout << "coordinates are   " << position.x << "   " << position.y << "   " << position.z <<
        "  time passed is: " << timePassed <<
        "  rotation = " << rotationX << "   " << rotationY << "\n";*/

    if (inputHandler.cursorIsEnabled() == false) {
        rotationX += inputHandler.mouseMovementX * sensitivity;
        rotationY += -inputHandler.mouseMovementY * sensitivity;
    }

    rotationY = std::clamp(rotationY, glm::radians(-89.0f), glm::radians(89.0f));
}

void CameraHandler::getCameraMatrix(VkExtent2D swapChainExtent, CameraUniformBufferObject& ubo)
{

    glm::mat4x4 cameraView =
        glm::lookAt(glm::vec3(position.x, position.y, position.z),
                    glm::vec3(position.x, position.y, position.z) + cameraForwardDirection(),
                    glm::vec3(0.0f, -1.0f, 0.0f) // up is down and the screen is not flipped like usual to make the
                                                 // positive right axis appear on the right of the screen.
        );
    glm::mat4x4 cameraProj =
        glm::perspective(fovY, swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 4000.0f);

    ubo.camera = cameraProj * cameraView;
    // ubo.camera[1][1] *= -1;

    const float rounding = 10.0f;
    glm::vec3 playerPosRounded = glm::vec3(round(position.x / rounding) * rounding,
                                           round(position.y / rounding) * rounding,
                                           round(position.z / rounding) * rounding);

    const glm::vec3 sunDir = glm::normalize(glm::vec3(0.3, -1, 0.5));
    ubo.sunDir = sunDir;
    const float boxRadius = 200.0f;
    glm::mat4x4 sunView =
        glm::lookAt(playerPosRounded + sunDir * -boxRadius,
                    playerPosRounded + sunDir * (-boxRadius + 1.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f) // up is down and the screen is not flipped like usual to make the
                                                 // positive right axis appear on the right of the screen.
        );
    glm::mat4x4 sunProj = glm::ortho(-boxRadius, boxRadius, -boxRadius, boxRadius, 1.0f, 2.0f * boxRadius);
    const glm::mat4 kBias = {0.5f,
                             0.0f,
                             0.0f,
                             0.0f,
                             0.0f,
                             0.5f,
                             0.0f,
                             0.0f,
                             0.0f,
                             0.0f,
                             1.0f,
                             0.0f, // z passes through untouched
                             0.5f,
                             0.5f,
                             0.0f,
                             1.0f};
    ubo.sun = sunProj * sunView;

    ubo.cameraToSun = kBias * ubo.sun;
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

ViewingFrustumNormals CameraHandler::getViewingFrustumNormals(VkExtent2D extent,
                                                              ViewingFrustumNormals& viewingFrustumNormals)
{
    glm::vec3 forwardDirection = cameraForwardDirection();
    glm::vec3 rightDirection = cameraRightDirection();
    glm::vec3 upDirection = cameraUpDirection();
    // glm::vec3 forwardDirection = {0, 0, 1};
    // glm::vec3 rightDirection = {1, 0, 0};
    // glm::vec3 upDirection = {0, 1, 0};

    viewingFrustumNormals.top = glm::rotate(forwardDirection, fovY / 2.0f + glm::radians(90.0f), -rightDirection);
    viewingFrustumNormals.bottom = glm::rotate(forwardDirection, fovY / 2.0f + glm::radians(90.0f), rightDirection);

    float fovX = glm::atan((float)extent.width / (float)extent.height * tan(fovY / 2.0f));

    viewingFrustumNormals.right = glm::rotate(forwardDirection, fovX + glm::radians(90.0f), upDirection);
    viewingFrustumNormals.left = glm::rotate(forwardDirection, fovX + glm::radians(90.0f), -upDirection);

    /*std::cout
        << " top = " << glm::dot(clipPlaneNormals.top, position)
        << " bottom = " << glm::dot(clipPlaneNormals.bottom, position)
        << " right = " << glm::dot(clipPlaneNormals.right, position)
        << " left = " << glm::dot(clipPlaneNormals.left, position)
        << "right vector is " << clipPlaneNormals.right.x << " " << clipPlaneNormals.right.y << " " <<
       clipPlaneNormals.right.z << "\n";*/

    return viewingFrustumNormals;
}
