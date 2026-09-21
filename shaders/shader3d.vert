#version 460

#extension GL_ARB_shading_language_packing : require
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require

layout(binding = 0) uniform UniformBufferObject {
    mat4 camera;
    mat4 sun;
    mat4 worldToSun;
    vec3 sunDir;
} ubo;

struct Vertex {
    uint pos;
    uint normal;
    float texLayer;
    uint uv;
};
layout(std430, buffer_reference, buffer_reference_align = 8) readonly buffer VertexBuffer {
    Vertex vertices[];
};

struct DrawData {
    vec3 chunkWorldLocation;
    float pad1;
};
layout(buffer_reference, std430, buffer_reference_align = 16) readonly buffer DrawDataBuffer {
    DrawData data[];
};

layout(push_constant) uniform constants
{
    VertexBuffer vertexBuffer;
    DrawDataBuffer drawDataBuffer;
} pc;

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out centroid vec2 outUV;
layout(location = 3) out float outTextureLayer;
layout(location = 4) out float outShadow;
layout(location = 5) out mat4 outWorldToSunMat;

void main()
{
    uint uint1 = pc.vertexBuffer.vertices[gl_VertexIndex].pos;
    vec3 pos = vec3(uint1 & 0x3FF, (uint1 >> 10) & 0x3FF, (uint1 >> 20) & 0x3FF);
    pos *= (1.0 / 16.0);
    vec3 chunkWorldLocation = pc.drawDataBuffer.data[gl_InstanceIndex].chunkWorldLocation;
    outPos = chunkWorldLocation + pos;
    gl_Position = ubo.camera * vec4(outPos, 1.0);

    uint uint2 = pc.vertexBuffer.vertices[gl_VertexIndex].normal;
    outNormal = unpackSnorm4x8(uint2).xyz;

    uint uint4 = pc.vertexBuffer.vertices[gl_VertexIndex].uv;
    outUV = vec2(uint4 & 0xFF, (uint4 >> 8) & 0xFF);

    outTextureLayer = pc.vertexBuffer.vertices[gl_VertexIndex].texLayer;
    outShadow = dot(ubo.sunDir, outNormal) > 0 ? 1.0 : 0.0;

    outWorldToSunMat = ubo.worldToSun;

    /*outPos = pushConstants.chunkWorldLocation + inVec1.xyz * 32.0;
    gl_Position = ubo.camera * vec4(outPos, 1.0);
    vec3 normal = vec3(inVec1.w, inVec2.xy) * 2 - 1;
    outNormal = normal;

    outUV = inTexturePlusShadow.xy * 32.0;
    outTextureLayer = inTexturePlusShadow.z * 1023.0;
    outShadow = dot(ubo.sunDir, normal) > 0 ? 1.0 : 0.0;
    outWorldToSunMat = ubo.worldToSun;*/
}
