#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uvCoords;

uniform mat4 modelMatrix;
uniform mat4 vpMatrix;

out vec2 tiledNormalUvCoords;
out vec2 tiledDuDvCoords;
out vec4 clipSpacePos;

out vec3 originalPos;

uniform float panner;

void main() {
	vec4 worldPos = modelMatrix * position;
	originalPos = worldPos.xyz;

	// Tile uvCoords
	tiledNormalUvCoords = uvCoords;
	tiledDuDvCoords = uvCoords;

	clipSpacePos = vpMatrix * worldPos;
	gl_Position = clipSpacePos;
}