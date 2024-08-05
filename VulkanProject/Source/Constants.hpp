#pragma once

#include <cstdint>

const int ITEM_STACK_MAX_SIZE = 256;
const int MAX_FRAMES_IN_FLIGHT = 2;
const int MAX_FPS = 200;

const uint32_t WIDTH = 1600;
const uint32_t HEIGHT = 900;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif