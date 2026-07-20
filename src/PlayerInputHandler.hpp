#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

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
    bool rHeld = false;
    bool rHeldPreviousFrame = false;
    bool rPressed = false;
    bool shiftHeld = false;
    bool ctrlHeld = false;
    bool f3Held = false;
    bool f3HeldPreviousFrame = false;
    bool f3Pressed = false;

    bool mouseLeftHeld = false;
    bool mouseLeftHeldPreviousFrame = false;
    bool mouseLeftPressed = false;
    bool mouseRightHeld = false;
    bool mouseRightHeldPreviousFrame = false;
    bool mouseRightPressed = false;

    double mouseLocationX = 0.0f;
    double mouseLocationY = 0.0f;
    double mouseMovementX = 0.0f;
    double mouseMovementY = 0.0f;
    glm::vec2 mouseLocationScreenSpace = {0.0f, 0.0f};

	void initGLFWControlCallbacks();
    void update();

    void enableCursor();
    void disableCursor();
    bool cursorIsEnabled();

    void handleMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void handleMouseMovement(GLFWwindow* window, double xpos, double ypos);
    void handleKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);

    static PlayerInputHandler& getInstance()
    {
        static PlayerInputHandler instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    PlayerInputHandler(PlayerInputHandler const&) = delete;
    void operator=(PlayerInputHandler const&) = delete;

private:
    uint32_t cursorEnabledRequestCount = 0;

    PlayerInputHandler() {}

    void toggleKeyHeld(int glfwKeyToCompare, bool& valueToModify, int key, int action);
    void toggleKeyOnPress(int glfwKeyToCompare, bool& previousFrameValue, bool& currentValue, bool& valueToModify);
};
