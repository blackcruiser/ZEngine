#version 450

layout(location = 0) in vec3 normal;
layout(location = 1) in vec2 texcoord;

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main()
 {
    outColor = texture(texSampler, texcoord);
}