#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(set = 0, binding = 0) uniform UniformBuffer
{ 
    mat4 mvp;
} ub;

layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outTexcoord;

void main()
{
    gl_Position = ub.mvp * vec4(position, 1.0) ;
    outNormal = normal;
    outTexcoord = texCoord;
}