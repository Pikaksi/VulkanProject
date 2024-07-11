#version 450

layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) centroid in vec2 fragTexCoord;
layout(location = 2) in float fragtexLayer;

layout(location = 0) out vec4 outColor;

void main() {
    //if (fragTexCoord.x >= 0.75f || fragTexCoord.y >= 0.75f || fragTexCoord.x <= 0.5f || fragTexCoord.y <= 0.5f) {
    //    outColor = vec4(1, 1, 1, 0);
    //}
    //else {
    //    outColor = vec4(1, 1, 0, 0);
    //}
    outColor = texture(texSampler, vec3(fragTexCoord, fragtexLayer)) * vec4(fragColor, 0);
}