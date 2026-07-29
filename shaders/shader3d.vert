#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 camera;
    mat4 sun;
    mat4 cameraToSun;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in float inTexLayer;
//layout(location = 4) in readonly buffer quadBuffer;

layout(push_constant) uniform constants
{
    vec3 chunkWorldLocation;
} pushConstant;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out float fragTexLayer;
layout(location = 3) out vec3 outWorldPosition;
layout(location = 4) out mat4 cameraToSunMat;

void main() {
    gl_Position = ubo.camera * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragTexLayer = inTexLayer;
    cameraToSunMat = ubo.cameraToSun;
    outWorldPosition = inPosition;
}
