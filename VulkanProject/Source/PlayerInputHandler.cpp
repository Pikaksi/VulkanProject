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
    toggleKeyHeld(GLFW_KEY_W, wHeld, key, action);
    toggleKeyHeld(GLFW_KEY_A, aHeld, key, action);
    toggleKeyHeld(GLFW_KEY_S, sHeld, key, action);
    toggleKeyHeld(GLFW_KEY_D, dHeld, key, action);
    toggleKeyHeld(GLFW_KEY_Q, qHeld, key, action);
    toggleKeyHeld(GLFW_KEY_E, eHeld, key, action);
    toggleKeyHeld(GLFW_KEY_LEFT_SHIFT, shiftHeld, key, action);
    toggleKeyHeld(GLFW_KEY_LEFT_CONTROL, ctrlHeld, key, action);

    toggleKeyHeld(GLFW_KEY_F3, f3Held, key, action);
}

void PlayerInputHandler::update()
{
    toggleKeyOnPress(GLFW_KEY_F3, f3HeldPreviousFrame, f3Held, f3Pressed);
}

void PlayerInputHandler::toggleKeyHeld(int glfwKeyToCompare, bool& valueToModify, int key, int action)
{
    if (key == glfwKeyToCompare && action == GLFW_PRESS) {
        valueToModify = true;
    }
    if (key == glfwKeyToCompare && action == GLFW_RELEASE) {
        valueToModify = false;
    }
}

void PlayerInputHandler::toggleKeyOnPress(int glfwKeyToCompare, bool& previousFrameValue, bool& currentValue, bool& valueToModify)
{
    if (previousFrameValue != currentValue && currentValue == true) {
        valueToModify = true;
    }
    else {
        valueToModify = false;
    }
    previousFrameValue = currentValue;
}