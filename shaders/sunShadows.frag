#version 450
layout(binding = 1) uniform sampler2DArray texSampler;

layout(location = 0) in vec2 inUV;

void main() {
    if (texture(texSampler, inUV).a < 0.5)   // 0.5 = your cutout threshold
        discard;
    // survivors write depth normally
}
