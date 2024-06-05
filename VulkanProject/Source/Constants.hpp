#pragma once

const int MAX_FRAMES_IN_FLIGHT = 2;

const uint32_t WIDTH = 1600;
const uint32_t HEIGHT = 900;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif