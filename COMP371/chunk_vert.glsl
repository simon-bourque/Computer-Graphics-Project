#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uvCoords;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 instancePosition;
layout(location = 4) in int faceIndex;

uniform mat4 vpMatrix;

uniform vec3 faceData[6]; // WARNING: This always has to match with the BlockType enum in Block.h

out VertexData {
	flat int faceIndex;
	vec2 UvCoords;
	vec3 normal; // normal in world space
	vec3 fragPos;
} passToFrag;

void main() {
	// If this is the top face
	if (normal == vec3(0.0,1.0,0.0)) {
			passToFrag.faceIndex = int(faceData[faceIndex].y);
	}
	// If this is the bottom face
	else if (normal == vec3(0.0,-1.0,0.0)) {
			passToFrag.faceIndex = int(faceData[faceIndex].z);
	}
	// If this is a side face
	else {
		passToFrag.faceIndex = int(faceData[faceIndex].x);
	}

	passToFrag.UvCoords = uvCoords;
	passToFrag.normal = normal; // No transformation needed because lighting is done in world space and our blocks cannot scale or rotate


	mat4 blockTransform = mat4(1.0); // Model matrix
	blockTransform[3][0] = instancePosition.x;
	blockTransform[3][1] = instancePosition.y;
	blockTransform[3][2] = instancePosition.z;
		
	passToFrag.fragPos = vec3(blockTransform * position);

	gl_Position = vpMatrix * blockTransform * position;
}
