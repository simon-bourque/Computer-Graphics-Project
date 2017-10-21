#version 430

layout(binding = 0) uniform sampler2DArray tilesheet;

flat in int passFaceIndex;
in vec2 passUvCoords;

out vec4 finalColor;

void main() {
	//finalColor = vec4(0.0, 0.0, 1.0, 1.0);
	finalColor = texture(tilesheet, vec3(passUvCoords.x, passUvCoords.y, passFaceIndex));
}
