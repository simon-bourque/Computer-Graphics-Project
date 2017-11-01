#version 430

uniform vec3 color;

out vec4 finalColor;

void main() {
	finalColor = vec4(color, 1.0);
}
