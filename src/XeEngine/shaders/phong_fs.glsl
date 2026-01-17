#version 460

layout(location=0) out vec4 vFragColor;

const int MAX_POINT_LIGHTS = 24;

#if __VERSION__ > 410
layout(std140, binding=0) uniform Color {
#else
    layout(std140) uniform Color {
    #endif
    vec4  Kd;
    bool use_map_Kd;
};

struct PointLight {
    vec3 position_in_view_space;
    vec3 color;
    float intensity;
    float radius;
};

#if __VERSION__ > 410
layout(std140, binding=2) uniform Lights {
#else
    layout(std140) uniform Lights {
    #endif
    vec3 ambient;
    uint n_p_lights;
    PointLight p_light[MAX_POINT_LIGHTS];
};

in vec2 vertex_texcoords_0;
in vec3 vertex_normals_in_vs;
in vec3 vertex_coords_in_vs;

uniform sampler2D map_Kd;

void main() {
    vec3 normal = normalize(vertex_normals_in_vs);
    
    vec4 Kd_color = Kd;
    if (use_map_Kd) {
        Kd_color = Kd * texture(map_Kd, vertex_texcoords_0);
    }
    
    vFragColor.a = Kd_color.a;
    vec3 result_color = Kd_color.rgb * ambient;
    
    for (uint i = 0; i < n_p_lights && i < MAX_POINT_LIGHTS; i++) {
        vec3 light_dir = p_light[i].position_in_view_space - vertex_coords_in_vs;
        float light_dist = length(light_dir);
        
        if (light_dist > 0.001) {
            vec3 L = normalize(light_dir);
            float diff = max(dot(normal, L), 0.0);
            result_color += Kd_color.rgb * p_light[i].color * p_light[i].intensity * diff;
        }
    }
    
    vFragColor.rgb = result_color;
}
