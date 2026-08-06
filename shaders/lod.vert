#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 camera;
    mat4 sun;
    mat4 worldToSun;
} ubo;

layout(location = 0) in vec4 inPosAndShadow;
layout(location = 1) in vec4 inColorAndNormal;

layout(push_constant) uniform constants
{
    vec3 chunkWorldLocation;
} pushConstants;

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outColor;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out float outShadow;
layout(location = 4) out mat4 outWorldToSunMat;

const vec3 faceNormals[6] = vec3[6](
    vec3( 1.0,  0.0,  0.0), vec3(-1.0,  0.0,  0.0),
    vec3( 0.0,  1.0,  0.0), vec3( 0.0, -1.0,  0.0),
    vec3( 0.0,  0.0,  1.0), vec3( 0.0,  0.0, -1.0));

void main() {
    outPos = pushConstants.chunkWorldLocation + inPosAndShadow.xyz * 32.0;
    gl_Position = ubo.camera * vec4(outPos, 1.0);
    outShadow = inPosAndShadow.w;

    outColor = inColorAndNormal.xyz;
    outNormal = faceNormals[uint(inColorAndNormal.w * 255.0 + 0.5)];

    outWorldToSunMat = ubo.worldToSun;
}
