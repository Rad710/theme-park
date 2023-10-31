#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube cubemap;

uniform vec4 color_diffuse = vec4(0.5, 0.5, 0.0, 1.0);
uniform float shininess = 77.0f;
uniform vec3 light_pos;
uniform vec3 EyeDirection = vec3(0.0, 0.0, 1.0);
uniform vec3 light_dir;

const vec3 UNSET_VALUE = vec3(-9999.0, -9999.0, -9999.0);

void main()
{
    vec3 normal = normalize(TexCoords - 0.5);

    vec3 light_direction;
    vec3 light_position;
    if (light_dir == UNSET_VALUE)
    {
        light_position = vec3(50.0f, 32.0f, 56.0f);
        light_direction = vec3(0.0f, -1.0f, 0.0f);
    }
    else
    {
        light_direction = light_dir;
        light_position = light_pos;
    }


    vec3 lightDir = normalize(light_position - TexCoords);
    vec3 viewDir = normalize(EyeDirection - TexCoords);

    // Calculate the reflection vector
    vec3 reflectDir = reflect(-lightDir, normal);

    // Calculate the diffuse component
    float diffuseFactor = max(dot(normal, lightDir), 0.0);
    vec4 diffuseColor = color_diffuse * diffuseFactor;

    // Calculate the final color using the ambient, diffuse, and specular components
    vec4 finalColor = diffuseColor;

    // Apply the light direction to the final texture
    vec4 texColor = texture(cubemap, TexCoords);

    // Mix the final color with the texture color

    if (light_dir == UNSET_VALUE)
    {
        FragColor = texture(cubemap, TexCoords);
    }
    else
    {
        FragColor = mix(finalColor, texColor, 0.5);
    }
}
