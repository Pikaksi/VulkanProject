#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include "VulkanRendering/Buffers.hpp"

struct ClipPlaneNormals
{
	glm::vec3 top;
	glm::vec3 bottom;
	glm::vec3 right;
	glm::vec3 left;
};

class CameraHandler
{
public:
	CameraHandler()
	{
		timeLastFrame = std::chrono::high_resolution_clock::now();
	}

	float fovY = glm::radians(85.0f);
	float sensitivity = 0.004f;
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
	void getCameraMatrix(VkExtent2D swapChainExtent, CameraUniformBufferObject& ubo);
	ClipPlaneNormals getClipPlaneNormals();
};