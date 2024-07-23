#version 450

layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) centroid in vec2 fragTexCoord;
layout(location = 2) in float fragtexLayer;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 tex = texture(texSampler, vec3(fragTexCoord, fragtexLayer));
    if (tex.w < 0.5) discard;
    outColor = tex * vec4(fragColor, 0);
}