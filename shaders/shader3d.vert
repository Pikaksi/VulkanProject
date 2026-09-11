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
    uint posAndNormal;
    uint normalAndUv;
    float texLayer;
    float pad;
};
layout(std430, buffer_reference, buffer_reference_align = 8) readonly buffer VertexBuffer {
    Vertex vertices[];
};

layout(push_constant) uniform constants
{
    vec3 chunkWorldLocation;
    VertexBuffer vertexBuffer;
} pc;

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out centroid vec2 outUV;
layout(location = 3) out float outTextureLayer;
layout(location = 4) out float outShadow;
layout(location = 5) out mat4 outWorldToSunMat;

void main()
{
    vec4 unpack1 = unpackUnorm4x8(pc.vertexBuffer.vertices[gl_VertexIndex].posAndNormal);
    vec3 pos = unpack1.xyz;

    uint unpack2 = pc.vertexBuffer.vertices[gl_VertexIndex].normalAndUv;
    outNormal = vec3(unpack1.w, unpackUnorm4x8(unpack2).xy) * 2.0 - 1.0;

    outUV = vec2((unpack2 >> 16) & 0x000000FF, (unpack2 >> 24) & 0x000000FF);

    outTextureLayer = pc.vertexBuffer.vertices[gl_VertexIndex].texLayer;
    outShadow = dot(ubo.sunDir, outNormal) > 0 ? 1.0 : 0.0;

    outPos = pc.chunkWorldLocation + pos * 32.0;
    gl_Position = ubo.camera * vec4(outPos, 1.0);

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
