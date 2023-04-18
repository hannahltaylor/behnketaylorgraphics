#version 410 core

in float Height;
in vec2 TextureCoord_TCS;
out vec4 FragColor;

uniform sampler2D rockTexture;
uniform sampler2D waterTexture;
uniform sampler2D snowTexture;


void main()
{
    float h = (Height + 16)/64.0f;
    // These are just numbers please play with them
    vec3 rColor = mix( texture(rockTexture, TextureCoord_TCS).rgb,   vec3(0,0,0), 0.7);
    vec3 wColor = mix(    texture(waterTexture, TextureCoord_TCS).rgb,  vec3(26/255.0f, 16/255.0f, 135/255.0f), 0.8);
    vec3 sColor = mix(    texture(snowTexture, TextureCoord_TCS).rgb,   vec3(1.0,1.0,1.0), 0.65);

    if(h > 0.27)
        FragColor = vec4(wColor, 1.0);
    else if(h > 0.265)
        FragColor = vec4(mix(rColor, wColor, 0.2), 1.0);
    else if(h < 0.258)
        FragColor = vec4(sColor, 1.0);
    else if(h < 0.2585)
        FragColor = vec4(mix(rColor, sColor, 0.9), 1.0);
    else    
        FragColor = vec4(rColor, 1.0);
    //FragColor = vec4(0.76*h, 0.699*h, 0.501*h, 1.0);
}