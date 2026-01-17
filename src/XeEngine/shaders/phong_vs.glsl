#version 460

layout(location=0) in vec4 a_vertex_position;
layout(location=1) in vec2 a_vertex_texcoords_0;
layout(location=2) in vec2 a_vertex_texcoords_1;
layout(location=3) in vec3 a_vertex_normals;

#if __VERSION__ > 410
layout(std140, binding=1) uniform Transformations {
#else
    layout(std140) uniform Transformations {
    #endif
    mat4 PVM;
    mat4 VM;
    mat3 N;
};

out vec2 vertex_texcoords_0;
out vec3 vertex_normals_in_vs;
out vec3 vertex_coords_in_vs;

void main() {
    vertex_texcoords_0 = a_vertex_texcoords_0;
    
    // Debug: Check if normals input is valid - if zero, output red-ish
    // vertex_normals_in_vs = a_vertex_normals;
    // if (length(a_vertex_normals) < 0.001) {
    //     vertex_normals_in_vs = vec3(1.0, 0.0, 0.0); // Red if no normals
    // }
    // return;
    
    // Transform vertex coordinates to view space
    vec4 vertex_coords_vs = VM * a_vertex_position;
    vertex_coords_in_vs = vertex_coords_vs.xyz / vertex_coords_vs.w;
    
    // Transform normals to view space and normalize
    vec3 transformed_normal = N * a_vertex_normals;
    if (length(transformed_normal) > 0.001) {
        vertex_normals_in_vs = normalize(transformed_normal);
    } else {
        vertex_normals_in_vs = vec3(0.0, 0.0, 1.0); // Default up normal if transformation fails
    }
    
    gl_Position =  PVM * a_vertex_position;
}