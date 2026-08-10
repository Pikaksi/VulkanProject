#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 camera;
    mat4 sun;
    mat4 worldToSun;
    vec3 sunDir;
} ubo;

layout(location = 0) in vec4 inVec1;
layout(location = 1) in vec2 inVec2;
layout(location = 2) in vec4 inTexturePlusShadow;
//layout(location = 4) in readonly buffer quadBuffer;

layout(push_constant) uniform constants
{
    vec3 chunkWorldLocation;
} pushConstants;

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out float outTextureLayer;
layout(location = 4) out float outShadow;
layout(location = 5) out mat4 outWorldToSunMat;

void main() {
    outPos = pushConstants.chunkWorldLocation + inVec1.xyz * 32.0;
    gl_Position = ubo.camera * vec4(outPos, 1.0);
    vec3 normal = vec3(inVec1.w, inVec2.xy) * 2 - 1;
    outNormal = normal;

    outUV = inTexturePlusShadow.xy * 32.0;
    outTextureLayer = inTexturePlusShadow.z * 1023.0;
    outShadow = dot(ubo.sunDir, normal) > 0 ? 1.0 : 0.0;
    outWorldToSunMat = ubo.worldToSun;
}
