#version 450

layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 0) in vec2 inUV;
layout(location = 1) in float inTexLayer;

void main() {
    vec4 tex = texture(texSampler, vec3(inUV, inTexLayer));
    if (tex.a < 0.5)
        discard;
    // survivors write depth normally
}
