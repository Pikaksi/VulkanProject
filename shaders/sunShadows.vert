#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 camera;
    mat4 sun;
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

void main() {
    gl_Position = ubo.sun * vec4(inPosition, 1.0);
}
