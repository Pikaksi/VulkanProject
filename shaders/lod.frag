#version 450

layout(binding = 1) uniform sampler2DShadow sunShadowSampler;

layout(location = 0) in vec3 inPos;
layout(location = 1) out vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in float inShadow;
layout(location = 4) in mat4 inWorldToSunMat;

layout(location = 0) out vec4 outColor;

float sampleShadow(vec3 viewPos) {
    viewPos = viewPos + inNormal.xyz * 0.1;
    vec4 c = inWorldToSunMat * vec4(viewPos, 1.0);
    c.xyz /= c.w;                        // no-op for ortho, harmless
    if (c.z > 1.0) return 1.0;           // beyond far plane: treat as lit
    if (c.x < 0.0 || c.x > 1.0 || c.y < 0.0 || c.y > 1.0) return 1.0;
    return texture(sunShadowSampler, vec3(c.x, c.y, c.z)); // first 2 params uv, last one depth to compare against
}

void main() {
    float shadow = 0.0;
    if (inShadow == 0) {
        shadow = sampleShadow(inPos);
    }
    color = vec4(color * (0.2 + 0.8 * shadow), 0);

    outColor = color;
}
