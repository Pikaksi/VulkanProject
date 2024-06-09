#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

class PlayerInputHandler
{
public:
	GLFWwindow* window;

	bool wHeld = false;
    bool aHeld = false;
    bool sHeld = false;
    bool dHeld = false;
    bool qHeld = false;
    bool eHeld = false;
    bool shiftHeld = false;
    bool ctrlHeld = false;
    bool f3Held = false;
    bool f3HeldPreviousFrame = false;
    bool f3Pressed = false;

    double mousePreviousLocationX = 0.0f;
    double mousePreviousLocationY = 0.0f;
    double mouseMovementX = 0.0f;
    double mouseMovementY = 0.0f;

	void initGLFWControlCallbacks();
    void update();
    void handleMouseMovement(GLFWwindow* window, double xpos, double ypos);
    void handleKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);
    void toggleKeyHeld(int glfwKeyToCompare, bool& valueToModify, int key, int action);
    void toggleKeyOnPress(int glfwKeyToCompare, bool& previousFrameValue, bool& currentValue, bool& valueToModify);

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
