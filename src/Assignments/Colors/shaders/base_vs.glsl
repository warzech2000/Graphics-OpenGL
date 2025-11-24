#version 420

layout(location=0) in vec3 a_vertex_position;
layout(location=1) in vec2 a_vertex_texcoord;
layout(location=2) in vec3 a_vertex_color;


out vec2 vertex_texcoords;
out vec3 vertexColor;

uniform Transformations {
    mat4 PVM;
};

void main() {
    gl_Position = PVM * vec4(a_vertex_position, 1.0);

    vertex_texcoords = a_vertex_texcoord;
    vertexColor = a_vertex_color;
}
