#version 150

in vec3 pos;
in vec2 texCoord;

out vec2 _texCoord;

void main() {
    gl_Position = vec4(pos, 1.0);
    _texCoord = texCoord;
}
