#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uvCoords;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 instancePosition;
layout(location = 4) in int faceIndex;

uniform mat4 vpMatrix;

uniform vec3 faceData[6]; // WARNING: This always has to match with the BlockType enum in Block.h

flat out int passFaceIndex;
out vec2 passUvCoords;
out vec3 passNormal;
out vec3 fragPos;

void main() {
	// If this is the top face
	if (normal == vec3(0.0,1.0,0.0)) {
			passFaceIndex = int(faceData[faceIndex].y);
	}
	// If this is the bottom face
	else if (normal == vec3(0.0,-1.0,0.0)) {
			passFaceIndex = int(faceData[faceIndex].z);
	}
	// If this is a side face
	else {
		passFaceIndex = int(faceData[faceIndex].x);
	}

	passUvCoords = uvCoords;
	passNormal = normal;

	mat4 blockTransform = mat4(1.0);
	blockTransform[3][0] = instancePosition.x;
	blockTransform[3][1] = instancePosition.y;
	blockTransform[3][2] = instancePosition.z;
	
	fragPos = vec3(blockTransform * position);
	gl_Position = vpMatrix * blockTransform * position;
}
