#version 450

layout(binding = 1) uniform sampler2DArray texSampler;
layout(binding = 2) uniform sampler2DShadow sunShadowSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) centroid in vec2 fragTexCoord;
layout(location = 2) in float fragtexLayer;
layout(location = 3) in vec3 worldPosition;
layout(location = 4) in mat4 cameraToSun;

layout(location = 0) out vec4 outColor;

/*float sampleShadow(vec3 viewPos, int cascade) {
    vec4 c = ubo.lightMatrix * vec4(viewPos, 1.0);
    c.xyz /= c.w;                        // no-op for ortho, harmless
    if (c.z > 1.0) return 1.0;           // beyond far plane: treat as lit
    // .xy = uv, .z = array layer, .w = depth to compare
    return texture(shadowMap, vec4(c.xy, float(cascade), c.z));
}*/

float sampleShadow(vec3 viewPos) {
    viewPos.y += 0.05;
    vec4 c = cameraToSun * vec4(viewPos, 1.0);
    c.xyz /= c.w;                        // no-op for ortho, harmless
    if (c.z > 1.0) return 1.0;           // beyond far plane: treat as lit
    if (c.x < 0.0 || c.x > 1.0 || c.y < 0.0 || c.y > 1.0) return 1.0;
    // .xy = uv, .z = depth to compare
    return texture(sunShadowSampler, vec3(c.x, c.y, c.z));
}

void main() {
    vec4 tex = texture(texSampler, vec3(fragTexCoord, fragtexLayer));

    //float shadow = texture(sunShadowSampler, vec3(fragTexCoord, 0.1));
    float shadow = sampleShadow(worldPosition);
    tex.xyz = tex.xyz * (0.2 + 0.8 * shadow);

    if (tex.w < 0.5) discard;
    outColor = tex * vec4(fragColor, 0);
}
