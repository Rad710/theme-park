#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texture_coordinate;

uniform mat4 u_model;

layout (std140, binding = 0) uniform common_matrices
{
    mat4 u_projection;
    mat4 u_view;
};

out V_OUT
{
   vec3 position;
   vec3 normal;
   vec2 texture_coordinate;
} v_out;

const float pi = 3.14159;

uniform float time;
uniform float speed;
uniform float amplitude;
uniform float wavelength;
uniform vec2 direction;

uniform sampler2D u_heightmap;

// Adjust this scaling factor to make the heightmap more noticeable
const float heightmapScale = 0.5;
// Adjust this factor to control the amount of random noise
const float noiseScale = 0.2;

// Function to generate random noise
float random(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

#define OCTAVES 6
float fbm (in vec2 st) {
    // Initial values
    float value = 0.0;
    float amplitude = .5;
    //
    // Loop of octaves
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(st);
        st *= 1.4;
        amplitude *= .5;
    }
    return value;
}

void main()
{
    vec3 p = position.xyz;

    float frequency = 2.0f * pi / wavelength;
    float c = sqrt(9.8 / frequency);
    vec2 d = normalize(direction);
    float f = frequency * (dot(d, p.xz) - c * time);

    vec2 heightmapCoord = vec2(texture_coordinate.x * wavelength, texture_coordinate.y * wavelength);
    // Add random noise to the height value
    float heightNoise = fbm(heightmapCoord);

    p.y += heightNoise * amplitude * sin(f);
    p.x += d.x * (amplitude * cos(f));
    p.z += d.y * (amplitude * cos(f));


    vec3 tangent = vec3(1 - d.x * d.x * (amplitude * sin(f) + heightNoise), d.x * (amplitude * cos(f)),
                        -d.x * d.y * (amplitude * sin(f) + heightNoise));

    vec3 binormal = vec3(-d.x * d.y * (amplitude * sin(f) + heightNoise), d.y * (amplitude * cos(f)),
                         1 - d.y * d.y * (amplitude * sin(f) + heightNoise));

    vec3 n = normalize(cross(binormal, tangent));

    gl_Position = u_projection * u_view * u_model * vec4(p, 1.0f);
    v_out.position = vec3(u_model * vec4(p, 1.0f));
    v_out.normal = n;
    v_out.texture_coordinate = vec2(texture_coordinate.x, 1.0f - texture_coordinate.y);
}
