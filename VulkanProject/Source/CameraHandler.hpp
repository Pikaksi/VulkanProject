#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

class CameraHandler
{
public:
	CameraHandler()
	{
		timeLastFrame = std::chrono::high_resolution_clock::now();
	}

	float sensitivity = 0.01f;
	float speedNormal = 4.0f;
	float slowerSpeedMultiplier = 5.0f;
	float fasterSpeedMultiplier = 20.0f;

	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

	float rotationX = 0.0f;
	float rotationY = 0.0f;

	std::chrono::steady_clock::time_point timeLastFrame;
	
	glm::vec3 cameraRightDirection();
	glm::vec3 cameraUpDirection();
	glm::vec3 cameraForwardDirection();
	void updateCameraTransform();
	UniformBufferObject getCameraMatrix(VkExtent2D swapChainExtent);
};