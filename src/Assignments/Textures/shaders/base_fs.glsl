#version 420

layout(location=0) out vec4 vFragColor;
in vec3 vertexColor;

layout(std140, binding = 0) uniform Modifier {
    float strength;
    vec3  color; 
   };

void main() {
    vec3 original = vertexColor; // or however you compute base color
    vec3 modified_rgb = original * strength * color;
    vFragColor = vec4(vertexColor,1.0);
}
