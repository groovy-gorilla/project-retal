#version 450

layout(location = 0) out vec3 color;

vec2 positions[3] = vec2[](
        vec2( 0.0,  0.5),  // góra
        vec2(-0.5, -0.5),  // lewy dół
        vec2( 0.5, -0.5)   // prawy dół
);

vec3 colors[3] = vec3[](
        vec3(1.0, 0.0, 0.0), // czerwony
        vec3(0.0, 1.0, 0.0), // zielony
        vec3(0.0, 0.0, 1.0)  // niebieski
);

void main()
{
    gl_Position = vec4(
            positions[gl_VertexIndex],
            0.0,
            1.0
    );

    color = colors[gl_VertexIndex];
}