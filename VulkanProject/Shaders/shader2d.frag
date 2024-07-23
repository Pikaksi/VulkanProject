#version 450

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float fragTexLayer;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 tex = texture(texSampler, fragTexCoord);
    if (tex.w < 0.5) discard;
    outColor = tex * outColor;
}