#version 450

layout(binding = 0) uniform sampler2D texSampler[8];

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) flat in uint fragTexLayer;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 tex = texture(texSampler[fragTexLayer], fragTexCoord);
    if (tex.w <= 0.0f) discard;

    //outColor = vec4(fragColor.w, 0.0f, 0.0f, 1.0f);
    outColor = tex * fragColor;
}