#version 450

layout(location = 0) in vec4 inValue;
layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(inValue.x, inValue.y, inValue.z, 1);
}