#version 410 core

in float Height;

out vec4 FragColor;

void main()
{
    float h = (Height + 16)/64.0f;
    if(h > 0.27)
        FragColor = vec4(0, 0, 1.0, 1.0);
    else
     FragColor = vec4(h, h, h, 1.0);
    //FragColor = vec4(0.76*h, 0.699*h, 0.501*h, 1.0);
}