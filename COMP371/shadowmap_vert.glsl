#version 430

layout(location = 0) in vec4 position;
layout(location = 3) in vec3 instancePosition;

uniform mat4 lightSpaceMatrix;

void main() {
	mat4 blockTransform = mat4(1.0); // Model matrix
	blockTransform[3][0] = instancePosition.x;
	blockTransform[3][1] = instancePosition.y;
	blockTransform[3][2] = instancePosition.z;

	gl_Position = lightSpaceMatrix * blockTransform * position;
}