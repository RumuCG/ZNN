#version 330 core

layout (location = 0) in vec3 aPos; // 顶点位置，绑定在位置0

void main()
{
    gl_Position = vec4(aPos, 1.0); // 把3D坐标变成4D，直接传给OpenGL
}
