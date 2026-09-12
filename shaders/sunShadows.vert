#version 460

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require

layout(binding = 0) uniform UniformBufferObject {
    mat4 camera;
    mat4 sun;
} ubo;

struct Vertex {
    uint pos;
    uint normal;
    float texLayer;
    uint uv;
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
    uint uint1 = pc.vertexBuffer.vertices[gl_VertexIndex].pos;
    vec3 pos = vec3(uint1 & 0x3FF, (uint1 >> 10) & 0x3FF, (uint1 >> 20) & 0x3FF);
    pos *= (1.0 / 16.0);
    vec3 outPos = pc.chunkWorldLocation + pos;
    gl_Position = ubo.sun * vec4(outPos, 1.0);

    outUV = vec2(0, 0);
    outTextureLayer = 0;
    //outUV = inTexPlusShadow.xy * 32.0;
    //outTextureLayer = inTexPlusShadow.z * 1023.0;
}

