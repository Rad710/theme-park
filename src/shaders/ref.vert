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

void main()
{
    vec3 p = position.xyz;

    float frequency = 2.0f * pi / wavelength;
    float c = sqrt(9.8 / frequency);
    vec2 d = normalize(direction);
    float f = frequency * (dot(d, p.xz) - c * time);

    p.y +=  amplitude * sin(f);
    p.x += d.x * (amplitude * cos(f));
    p.z += d.y * (amplitude * cos(f));

    vec3 tangent = normalize(vec3(1, frequency * amplitude * cos(f), 0));

    vec3 n = normalize(vec3(-tangent.y, tangent.x, 0));

    gl_Position = u_projection * u_view * u_model * vec4(p, 1.0f);
    v_out.position = vec3(u_model * vec4(p, 1.0f));
    v_out.normal = n;
    v_out.texture_coordinate = vec2(texture_coordinate.x, 1.0f - texture_coordinate.y);
}
