#version 330
in vec3 v_color;
out vec4 pixel;
void main() {
  pixel = vec4(v_color, 1.0);
}
