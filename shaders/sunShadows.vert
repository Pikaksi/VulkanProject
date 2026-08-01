#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 camera;
    mat4 sun;
} ubo;

layout(location = 0) in vec4 inVec1;
layout(location = 1) in vec2 inVec2;
layout(location = 2) in vec4 inTexPlusShadow;
//layout(location = 4) in readonly buffer quadBuffer;

layout(push_constant) uniform constants
{
    vec3 chunkWorldLocation;
} pushConstants;

layout(location = 0) out vec2 outUV;
layout(location = 1) out float outTextureLayer;

void main() {
    vec3 pos = pushConstants.chunkWorldLocation + inVec1.xyz * 32.0;
    gl_Position = ubo.sun * vec4(pos, 1.0);

    outUV = inTexPlusShadow.xy * 32.0;
    outTextureLayer = inTexPlusShadow.z * 1023.0;
}
