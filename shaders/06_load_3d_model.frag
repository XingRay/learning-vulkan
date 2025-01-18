#version 450

layout (location = 0) in vec2 fragTexCoord;

layout (set = 0, binding = 1) uniform sampler2D texSampler;

layout (location = 0) out vec4 outColor;

void main() {
        outColor = texture(texSampler, fragTexCoord);
//    outColor = vec4(fragTexCoord, 0.0, 1.0);
}