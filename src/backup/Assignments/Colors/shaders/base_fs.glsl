#version 420

layout(location=0) out vec4 vFragColor;
in vec3 vertexColor;

void main() {
    vFragColor = vec4(vertexColor,1.0);
}
