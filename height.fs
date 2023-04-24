#version 410 core

in float Height;
in vec2 TextureCoord_TCS;
in vec3 te_position;
in vec4 te_normal;
out vec4 FragColor;


uniform sampler2D rockTexture;
uniform sampler2D waterTexture;
uniform sampler2D snowTexture;


vec3 PhongBlinn(vec3 inColor, float a) {
        // Phong Shading + Blinn-Phong For specular
        vec3 lightPos = vec3(0.0f, 500.0f, 0.0f);
        vec3 viewPos = vec3(0.5f, 1.0f, 1.0f);
        vec3 ambient = a * inColor;
        vec3 lightDir = normalize(lightPos - te_position);
        vec3 normal = normalize(te_normal.xyz);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = diff * inColor;
        vec3 viewDir = normalize(viewPos - te_position);
        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 halfwayDir = normalize(lightDir + viewDir); 
        float spec;  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
        vec3 specular = vec3(0.3) * spec;
        vec3 final = ambient+diffuse+specular;
        return final;
    }

void main()
{
    float h = (Height + 16)/64.0f;
    // These are just numbers please play with them
    vec3 rColor = mix( texture(rockTexture, TextureCoord_TCS).rgb,   vec3(0,0,0), 0.7);
    vec3 wColor = mix(    texture(waterTexture, TextureCoord_TCS).rgb,  vec3(26/255.0f, 16/255.0f, 135/255.0f), 0.95);
    vec3 sColor = mix(    texture(snowTexture, TextureCoord_TCS).rgb,   vec3(1.0,1.0,1.0), 0.65);

    if(h > 0.271)
        FragColor = vec4(PhongBlinn(wColor, 0.05), 1.0);
    else if(h > 0.268)
        FragColor = vec4(PhongBlinn(mix(rColor, wColor, 0.2), 0.15), 1.0);
    else if(h < 0.258)
        FragColor = vec4(PhongBlinn(sColor, 0.25), 1.0);
    else if(h < 0.2585)
        FragColor = vec4(PhongBlinn(mix(rColor, sColor, 0.9), 0.35), 1.0);
    else    
        FragColor = vec4(PhongBlinn(rColor, 0.45), 1.0);
    //FragColor = vec4(0.76*h, 0.699*h, 0.501*h, 1.0);
}