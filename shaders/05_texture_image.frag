#version 460

layout (location = 0) in vec2 inTextureCoordinates;

layout (binding = 0) uniform sampler2D textureSampler;

layout (location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(1.0, 0.0, 0.0, 1.0); // test
    //outColor = vec4(inTextureCoordinates, 0.0, 1.0); // test
    outColor = texture(textureSampler, inTextureCoordinates);
}