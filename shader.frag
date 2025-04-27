#version 330 core  // 声明GLSL版本

out vec4 FragColor; // 输出颜色（新版写法）
uniform float utime;
void main()
{
    FragColor = vec4(0+utime,1.0,1.0,1.0); // 纯红色 (RGBA)
}
