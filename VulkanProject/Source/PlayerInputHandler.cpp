#include "PlayerInputHandler.hpp"

#include <iostream>

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    PlayerInputHandler::getInstance().handleKeyPress(window, key, scancode, action, mods);
}

void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{
    PlayerInputHandler::getInstance().handleMouseMovement(window, xPos, yPos);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    PlayerInputHandler::getInstance().handleMouseButtonCallback(window, button, action, mods);
}

void PlayerInputHandler::initGLFWControlCallbacks()
{
	glfwSetKeyCallback(window, keyCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
}

void PlayerInputHandler::enableCursor()
{
    cursorEnabledRequestCount += 1;
    if (cursorEnabledRequestCount == 1) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        glfwSetCursorPos(window, width / 2, height / 2);
    }
}

void PlayerInputHandler::disableCursor()
{
    if (cursorEnabledRequestCount == 0) {
        throw std::runtime_error("requested to disable cursor when it was already disabled");
    }

    cursorEnabledRequestCount -= 1;
    if (cursorEnabledRequestCount == 0) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

bool PlayerInputHandler::cursorIsEnabled()
{
    return cursorEnabledRequestCount != 0;
}

void PlayerInputHandler::handleMouseMovement(GLFWwindow* window, double xPos, double yPos)
{
    if (!cursorIsEnabled()) {
        mouseMovementX = xPos - mouseLocationX;
        mouseMovementY = yPos - mouseLocationY;
    }

    //std::cout << "mouse movement is " << mouseMovementX << "  " << mouseMovementY << "\n";

    mouseLocationX = xPos;
    mouseLocationY = yPos;
}

void PlayerInputHandler::handleMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    toggleKeyHeld(GLFW_MOUSE_BUTTON_LEFT, mouseLeftHeld, button, action);
    toggleKeyHeld(GLFW_MOUSE_BUTTON_LEFT, mouseRightHeld, button, action);
}

void PlayerInputHandler::handleKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    toggleKeyHeld(GLFW_KEY_W, wHeld, key, action);
    toggleKeyHeld(GLFW_KEY_A, aHeld, key, action);
    toggleKeyHeld(GLFW_KEY_S, sHeld, key, action);
    toggleKeyHeld(GLFW_KEY_D, dHeld, key, action);
    toggleKeyHeld(GLFW_KEY_Q, qHeld, key, action);
    toggleKeyHeld(GLFW_KEY_E, eHeld, key, action);
    toggleKeyHeld(GLFW_KEY_R, rHeld, key, action);
    toggleKeyHeld(GLFW_KEY_LEFT_SHIFT, shiftHeld, key, action);
    toggleKeyHeld(GLFW_KEY_LEFT_CONTROL, ctrlHeld, key, action);
    toggleKeyHeld(GLFW_KEY_F3, f3Held, key, action);
}

void PlayerInputHandler::update()
{
    toggleKeyOnPress(GLFW_KEY_F3, rHeldPreviousFrame, rHeld, rPressed);
    toggleKeyOnPress(GLFW_KEY_F3, f3HeldPreviousFrame, f3Held, f3Pressed);

    toggleKeyOnPress(GLFW_KEY_F3, mouseLeftHeldPreviousFrame, mouseLeftHeld, mouseLeftPressed);
    toggleKeyOnPress(GLFW_KEY_F3, mouseRightHeldPreviousFrame, mouseRightHeld, mouseRightPressed);
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