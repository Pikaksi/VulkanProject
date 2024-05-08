#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

class PlayerInputHandler
{
public:
	GLFWwindow* window;

	bool wPressed = false;
    bool aPressed = false;
    bool sPressed = false;
    bool dPressed = false;
    bool qPressed = false;
    bool ePressed = false;
    bool shiftPressed = false;
    bool ctrlPressed = false;

    double mousePreviousLocationX = 0.0f;
    double mousePreviousLocationY = 0.0f;
    double mouseMovementX = 0.0f;
    double mouseMovementY = 0.0f;

	void initGLFWControlCallbacks();
    void handleMouseMovement(GLFWwindow* window, double xpos, double ypos);
    void handleKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
    void toggleKey(int glfwKeyToCompare, bool& valueToModify, int key, int action);

    static PlayerInputHandler& getInstance()
    {
        static PlayerInputHandler    instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    PlayerInputHandler(PlayerInputHandler const&) = delete;
    void operator=(PlayerInputHandler const&) = delete;

private:
    PlayerInputHandler() {}
};
