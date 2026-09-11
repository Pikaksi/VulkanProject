#include "PlayerInputHandler.hpp"
#include "assertm.hpp"

#include <iostream>

InputHandler inputHandler = InputHandler();

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_DONT_CARE) return;
    assertm(key <= GLFW_KEY_LAST, "glfw key callback gave a bad key somehow");
    if (action == GLFW_PRESS) {
        inputHandler.keyboardKeyStates[key].held = true;
    }
    else if (action == GLFW_RELEASE) {
        inputHandler.keyboardKeyStates[key].held = false;
    }
}

void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos)
{
    inputHandler.mouseLocationX = xPos;
    inputHandler.mouseLocationY = yPos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        inputHandler.mouseKeyStates[button].held = true;
    }
    else if (action == GLFW_RELEASE) {
        inputHandler.mouseKeyStates[button].held = false;
    }
}

void focusCallback(GLFWwindow* window, int focused)
{
    if (focused) {
        // The window gained focus back
        inputHandler.framesSinceFocusedIn = 0;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else {
        // The window lost focus (tabbed out)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void InputHandler::initGLFWControlCallbacks()
{
    glfwSetWindowFocusCallback(window, focusCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window, keyCallback);

    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
}

void InputHandler::enableCursor()
{
    cursorEnabledRequestCount += 1;
    if (cursorEnabledRequestCount == 1) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        glfwSetCursorPos(window, width / 2, height / 2);
    }
}

void InputHandler::disableCursor()
{
    assertm(cursorEnabledRequestCount != 0, "requested to disable cursor when it was already disabled");

    cursorEnabledRequestCount -= 1;
    if (cursorEnabledRequestCount == 0) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

bool InputHandler::cursorIsEnabled() { return cursorEnabledRequestCount != 0; }
bool InputHandler::keyPressed(int key)
{
    assertm(key <= GLFW_KEY_LAST, "Key code for keyboard does not exist");
    return keyboardKeyStates[key].pressed;
}
bool InputHandler::keyHeld(int key) { return keyboardKeyStates[key].held; }
bool InputHandler::mousePressed(int key) { 
    assertm(key <= GLFW_MOUSE_BUTTON_LAST, "Key code for mouse does not exist");
    return mouseKeyStates[key].pressed; }
bool InputHandler::mouseHeld(int key) { return mouseKeyStates[key].held; }

void InputHandler::update()
{
    for (int i = 0; i < keyboardKeyCount; i++) {
        KeyState& keyState = keyboardKeyStates[i];
        keyState.pressed = keyState.held && !keyState.heldPreviousFrame;
        keyState.heldPreviousFrame = keyState.held;
    }

    for (int i = 0; i < mouseKeyCount; i++) {
        KeyState& keyState = mouseKeyStates[i];
        keyState.pressed = keyState.held && !keyState.heldPreviousFrame;
        keyState.heldPreviousFrame = keyState.held;
    }

    if (framesSinceFocusedIn > 3) {
        mouseMovementX = -mousePreviousLocationX + mouseLocationX;
        mouseMovementY = -mousePreviousLocationY + mouseLocationY;
    }
    else {
        mouseMovementX = 0;
        mouseMovementY = 0;
    }
    mousePreviousLocationX = mouseLocationX;
    mousePreviousLocationY = mouseLocationY;
    framesSinceFocusedIn++;
}
