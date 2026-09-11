#version 460

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require

layout(binding = 0) uniform UniformBufferObject {
    mat4 camera;
    mat4 sun;
} ubo;

struct Vertex {
    uint position;
    uint uv;
    uint normal;
    uint padding;
};
layout(buffer_reference, std430, buffer_reference_align = 8) readonly buffer VertexBuffer {
    Vertex vertices[];
};

layout(push_constant) uniform constants
{
    vec3 chunkWorldLocation;
    VertexBuffer vertexBuffer;
} pc;

layout(location = 0) out vec2 outUV;
layout(location = 1) out float outTextureLayer;

void main() {
    uint param1 = pc.vertexBuffer.vertices[gl_VertexIndex].position;
    vec4 positionNormal = unpackUnorm4x8(param1);
    vec3 worldPosition = vec3(positionNormal);
    vec3 outPos = pc.chunkWorldLocation + worldPosition * 32.0;
    gl_Position = ubo.sun * vec4(outPos, 1.0);

    outUV = vec2(0, 0);
    outTextureLayer = 0;
    //outUV = inTexPlusShadow.xy * 32.0;
    //outTextureLayer = inTexPlusShadow.z * 1023.0;
}
