#version 430 core
out vec4 f_color;

in V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
} f_in;

uniform sampler2D u_texture;

uniform vec4 color_ambient = vec4(0.1, 0.2, 0.5, 1.0);
uniform vec4 color_diffuse = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 color_specular = vec4(2.0, 2.0, 2.0, 1.0);
uniform float shininess = 77.0f;
uniform vec3 light_pos;
uniform vec3 EyeDirection = vec3(0.0, 0.0, 1.0);

uniform vec3 light_dir;
const vec3 UNSET_VALUE = vec3(-9999.0, -9999.0, -9999.0);

uniform float time;
uniform float wavelength;
uniform float amplitude;

uniform samplerCube skybox;
//uniform samplerCube tiles;


layout (std140, binding = 0) uniform common_matrices
{
    mat4 u_projection;
    mat4 u_view;
};

void main()
{   
    mat4 inverseViewMatrix = inverse(u_view);
    vec3 cameraPos = inverseViewMatrix[3].xyz;
    vec3 I = normalize(f_in.position - cameraPos);

    vec3 normal = normalize(f_in.normal);

    vec3 reflectionVector = reflect(I, normal);
    vec3 reflectedTexture = texture(skybox, reflectionVector).rgb;

    // Calculate a time-dependent offset for the refraction effect
    
    //float ratio = 1.01;
    //vec3 refractionVector = refract(I, normal, ratio);
    //vec3 refractedTexture = texture(tiles, refractionVector).rgb;

    vec3 light_direction;
    vec3 light_position;
    if (light_dir == UNSET_VALUE)
    {
        light_position = vec3(50.0f, 32.0f, 56.0f);
        light_direction = normalize(light_position - f_in.position);
    }
    else
    {
        light_direction = light_dir;
        light_position = light_pos;
    }

    vec3 half_vector = normalize(light_direction + normalize(EyeDirection));
    
    float diffuse = max(0.0, dot(normal, light_direction));
    
    float specular = pow(max(0.0, dot(normal, half_vector)), shininess);
    
    vec4 lighting = color_ambient + diffuse * color_diffuse + specular * color_specular;

    vec3 color_lighting = lighting.xyz / lighting.w;

    //vec3 mixed = mix(reflectedTexture, refractedTexture, 0.2f);
    //vec3 textureColor = texture(u_texture, f_in.texture_coordinate).rgb;
    vec3 blendedColor = mix(reflectedTexture, color_lighting, 0.5f); // Blend texture color with vertex color using a 50% weight

    // Mix the reflected color with the blended color using a 50% weight
    //vec3 finalColor = mix(blendedColor, reflectedColor, 0.3f);

    f_color = vec4(blendedColor, 1.0f);
}
