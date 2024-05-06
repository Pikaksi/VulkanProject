#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

#include "PlayerInputHandler.hpp"

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    PlayerInputHandler::getInstance().handleKeyPress(window, key, scancode, action, mods);
}

void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{
    PlayerInputHandler::getInstance().handleMouseMovement(window, xPos, yPos);
}

void PlayerInputHandler::initGLFWControlCallbacks()
{
	glfwSetKeyCallback(window, keyCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
}

void PlayerInputHandler::handleMouseMovement(GLFWwindow* window, double xPos, double yPos)
{
    mouseMovementX = xPos - mousePreviousLocationX;
    mouseMovementY = yPos - mousePreviousLocationY;

    //std::cout << "mouse movement is " << mouseMovementX << "  " << mouseMovementY << "\n";

    mousePreviousLocationX = xPos;
    mousePreviousLocationY = yPos;
}

void PlayerInputHandler::handleKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    toggleKey(GLFW_KEY_W, wPressed, key, action);
    toggleKey(GLFW_KEY_A, aPressed, key, action);
    toggleKey(GLFW_KEY_S, sPressed, key, action);
    toggleKey(GLFW_KEY_D, dPressed, key, action);
    toggleKey(GLFW_KEY_Q, qPressed, key, action);
    toggleKey(GLFW_KEY_E, ePressed, key, action);
    toggleKey(GLFW_KEY_LEFT_SHIFT, shiftPressed, key, action);
}

void PlayerInputHandler::toggleKey(int glfwKeyToCompare, bool& valueToModify, int key, int action)
{
    if (key == glfwKeyToCompare && action == GLFW_PRESS) {
        valueToModify = true;
    }
    if (key == glfwKeyToCompare && action == GLFW_RELEASE) {
        valueToModify = false;
    }
}