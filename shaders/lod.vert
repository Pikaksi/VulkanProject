#version 460

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require

layout(binding = 0) uniform UniformBufferObject {
    mat4 camera;
    mat4 sun;
    mat4 worldToSun;
    vec3 sunDir;
} ubo;

struct Vertex {
    uint position;
    uint colorNormal;
};
layout(buffer_reference, std430, buffer_reference_align = 8) readonly buffer VertexBuffer {
    Vertex vertices[];
};

layout(push_constant) uniform constants
{
    vec3 chunkWorldLocation;
    float chunkSize;
    VertexBuffer vertexBuffer;
} pc;

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outColor;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out float outShadow;
layout(location = 4) out mat4 outWorldToSunMat;

const vec3 faceNormals[6] = vec3[6](
    vec3( 1.0,  0.0,  0.0), vec3(-1.0,  0.0,  0.0),
    vec3( 0.0,  1.0,  0.0), vec3( 0.0, -1.0,  0.0),
    vec3( 0.0,  0.0,  1.0), vec3( 0.0,  0.0, -1.0));

/*vec4 unpackA2B10G10R10(uint p)
{
    // 1. Extract the integer bits using bitshifts and masks
    // 0x3FF is 1023 in hex (10 bits of 1s)
    // 0x3 is 3 in hex (2 bits of 1s)
    uvec4 bits = uvec4(
        p & 0x3FFu,             // R: bits 0-9
        (p >> 10) & 0x3FFu,     // G: bits 10-19
        (p >> 20) & 0x3FFu,     // B: bits 20-29
        (p >> 30) & 0x3u        // A: bits 30-31
    );

    // 2. Cast to float and normalize
    // 10-bit max is 1023.0, 2-bit max is 3.0
    return vec4(bits) * vec4(
        1.0 / 1023.0, 
        1.0 / 1023.0, 
        1.0 / 1023.0, 
        1.0 / 3.0
    );
}*/

void main() {
    vec3 inPositionAndShadow = unpackUnorm4x8(pc.vertexBuffer.vertices[gl_VertexIndex].position).xyz;
    outPos = pc.chunkWorldLocation + inPositionAndShadow.xyz * pc.chunkSize;

    outWorldToSunMat = ubo.worldToSun;

    outNormal = vec3(1.0, 0, 0);
    //outNormal = faceNormals[uint(inColorAndNormal.w * 255.0 + 0.5)];
    outShadow = dot(ubo.sunDir, outNormal) > 0 ? 1.0 : 0.0;

    outColor = vec3(0.5, 0.5, 0.5);

    /*outPos = pushConstants.chunkWorldLocation + inPosAndShadow.xyz * pushConstants.chunkSize;
    gl_Position = ubo.camera * vec4(outPos, 1.0);
    vec3 normal = faceNormals[uint(inColorAndNormal.w * 255.0 + 0.5)];
    outNormal = normal;
    outShadow = dot(ubo.sunDir, normal) > 0 ? 1.0 : 0.0;

    outColor = inColorAndNormal.xyz;*/
}
