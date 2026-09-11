#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

struct KeyState
{
    bool held = false;
    bool heldPreviousFrame = false;
    bool pressed = false;
};

struct InputHandler
{
    GLFWwindow* window;

    static constexpr int keyboardKeyCount = GLFW_KEY_LAST + 1;
    KeyState keyboardKeyStates[keyboardKeyCount];
    static constexpr int mouseKeyCount = GLFW_MOUSE_BUTTON_LAST + 1;
    KeyState mouseKeyStates[keyboardKeyCount];

    int framesSinceFocusedIn = 0;
    uint32_t cursorEnabledRequestCount = 0;

    double mousePreviousLocationX = 0.0f;
    double mousePreviousLocationY = 0.0f;
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

    bool keyPressed(int key);
    bool keyHeld(int key);
    bool mousePressed(int key);
    bool mouseHeld(int key);

    InputHandler() {}
};

extern InputHandler inputHandler;

    /*bool numHeld[10]{};
    bool numHeldPreviousFrame[10]{};
    bool numPressed[10]{};
    bool oneHeld = false;
    bool twoHeld = false;
    bool threeHeld = false;
    bool fourHeld = false;
    bool fiveHeld = false;
    bool sixHeld = false;
    bool sevenHeld = false;
    bool eightHeld = false;
    bool nineHeld = false;
    bool zeroHeld = false;
    bool oneHeldPreviousFrame = false;
    bool twoHeldPreviousFrame = false;
    bool threeHeldPreviousFrame = false;
    bool fourHeldPreviousFrame = false;
    bool fiveHeldPreviousFrame = false;
    bool sixHeldPreviousFrame = false;
    bool sevenHeldPreviousFrame = false;
    bool eightHeldPreviousFrame = false;
    bool nineHeldPreviousFrame = false;
    bool zeroHeldPreviousFrame = false;
    bool onePressed = false;
    bool twoPressed = false;
    bool threePressed = false;
    bool fourPressed = false;
    bool fivePressed = false;
    bool sixPressed = false;
    bool sevenPressed = false;
    bool eightPressed = false;
    bool ninePressed = false;
    bool zeroPressed = false;

    bool wHeld = false;
    bool aHeld = false;
    bool sHeld = false;
    bool dHeld = false;
    bool qHeld = false;
    bool cHeld = false;
    bool vHeld = false;
    bool altHeld = false;
    bool shiftHeld = false;
    bool ctrlHeld = false;

    bool spaceHeld = false;
    bool spaceHeldPreviousFrame = false;
    bool spacePressed = false;

    bool eHeld = false;
    bool eHeldPreviousFrame = false;
    bool ePressed = false;

    bool rHeld = false;
    bool rHeldPreviousFrame = false;
    bool rPressed = false;

    bool f3Held = false;
    bool f3HeldPreviousFrame = false;
    bool f3Pressed = false;

    bool f4Held = false;
    bool f4HeldPreviousFrame = false;
    bool f4Pressed = false;

    bool mouseLeftHeld = false;
    bool mouseLeftHeldPreviousFrame = false;
    bool mouseLeftPressed = false;
    bool mouseRightHeld = false;
    bool mouseRightHeldPreviousFrame = false;
    bool mouseRightPressed = false;*/
