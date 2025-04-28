#version 330 core  // 声明GLSL版本
in vec2 TexCoord;
out vec4 FragColor; // 输出颜色（新版写法）
uniform float utime;
uniform sampler2D texture0;
void main()
{
    //FragColor = vec4(0+utime,1.0,1.0,1.0); // 纯红色 (RGBA)
    FragColor = texture(texture0, TexCoord);
}

