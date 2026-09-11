#version 460

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : require

layout(binding = 0) uniform UniformBufferObject {
    mat4 camera;
    mat4 sun;
    mat4 worldToSun;
    vec3 sunDir;
} ubo;

struct Vertex {
    uint position;
    uint uv;
    uint normal;
    uint padding;
};
layout(std430, buffer_reference, buffer_reference_align = 4) readonly buffer VertexBuffer {
    Vertex vertices[];
};

layout(push_constant) uniform constants
{
    vec3 chunkWorldLocation;
    VertexBuffer vertexBuffer;
} pc;

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;
layout(location = 3) out float outTextureLayer;
layout(location = 4) out float outShadow;
layout(location = 5) out mat4 outWorldToSunMat;

void main() {

    uint param1 = pc.vertexBuffer.vertices[gl_VertexIndex].position;
    vec4 positionNormal = unpackUnorm4x8(param1);
    vec3 worldPosition = vec3(positionNormal);
    outPos = pc.chunkWorldLocation + worldPosition * 32.0;
    gl_Position = ubo.camera * vec4(outPos, 1.0);

    outWorldToSunMat = ubo.worldToSun;

    outNormal = vec3(0, 0, 0);
    outUV = vec2(0, 0);
    outTextureLayer = 0;
    outShadow = 0;

    /*outPos = pushConstants.chunkWorldLocation + inVec1.xyz * 32.0;
    gl_Position = ubo.camera * vec4(outPos, 1.0);
    vec3 normal = vec3(inVec1.w, inVec2.xy) * 2 - 1;
    outNormal = normal;

    outUV = inTexturePlusShadow.xy * 32.0;
    outTextureLayer = inTexturePlusShadow.z * 1023.0;
    outShadow = dot(ubo.sunDir, normal) > 0 ? 1.0 : 0.0;
    outWorldToSunMat = ubo.worldToSun;*/
}
