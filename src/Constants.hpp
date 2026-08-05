#pragma once

#include <cstdint>

const int ITEM_STACK_MAX_SIZE = 256;
const int MAX_FRAMES_IN_FLIGHT = 2;

const uint32_t UI_GPU_BUFFER_SIZE = 1 * 1024 * 1024;
const uint32_t INDEX_BUFFER_QUAD_COUNT = 100000;

const uint32_t WIDTH = 1600;
const uint32_t HEIGHT = 900;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
