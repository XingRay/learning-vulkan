#version 460
layout (location = 0) in vec3 inPosition;
layout (location = 0) out vec3 fragColor;

layout (set = 0, binding = 0) uniform TransformUniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} transformUbo;

layout (set = 0, binding = 1) uniform ColorUniformBufferObject {
    vec3 color;
} colorUbo;

void main() {
    gl_Position = transformUbo.proj * transformUbo.view * transformUbo.model * vec4(inPosition, 1.0);
    fragColor = colorUbo.color;
}
