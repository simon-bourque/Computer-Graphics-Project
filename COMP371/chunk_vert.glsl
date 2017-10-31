#version 430

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uvCoords;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 instancePosition;
layout(location = 4) in int faceIndex;

uniform mat4 vpMatrix;
uniform mat4 viewMatrix;

uniform vec3 faceData[6]; // WARNING: This always has to match with the BlockType enum in Block.h

//flat out int passFaceIndex;
//out vec2 passUvCoords;
//out vec3 passNormal;
//out vec3 fragPos;

out vertexData {
	flat int faceIndex;
	vec2 UvCoords;
	vec3 normal;
	vec3 fragPos;
} passToGeo;

void main() {
	// If this is the top face
	if (normal == vec3(0.0,1.0,0.0)) {
			passToGeo.faceIndex = int(faceData[faceIndex].y);
	}
	// If this is the bottom face
	else if (normal == vec3(0.0,-1.0,0.0)) {
			passToGeo.faceIndex = int(faceData[faceIndex].z);
	}
	// If this is a side face
	else {
		passToGeo.faceIndex = int(faceData[faceIndex].x);
	}

	passToGeo.UvCoords = uvCoords;
	vec4 tempNormal = viewMatrix * vec4(normal, 0.0);
	passToGeo.normal = normalize(tempNormal.xyz);
	//passToGeo.normal = normal;

	mat4 blockTransform = mat4(1.0);
	blockTransform[3][0] = instancePosition.x;
	blockTransform[3][1] = instancePosition.y;
	blockTransform[3][2] = instancePosition.z;
	
	passToGeo.fragPos = vec3(blockTransform * position);
	gl_Position = vpMatrix * blockTransform * position;
}
