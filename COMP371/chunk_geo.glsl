#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
//layout(line_strip, max_vertices = 6) out;

in vertexData {
	flat int faceIndex;
	vec2 UvCoords;
	vec3 normal;
	vec3 fragPos;
} passFromVertex[];

out fragData {
	flat int faceIndex;
	vec2 UvCoords;
	vec3 normal;
	vec3 fragPos;
} passToFrag;

void main() {
	for (int i = 0; i < gl_in.length(); i++) {
		passToFrag.faceIndex = passFromVertex[i].faceIndex;
		passToFrag.UvCoords = passFromVertex[i].UvCoords;
		passToFrag.normal = passFromVertex[i].normal;
		passToFrag.fragPos = passFromVertex[i].fragPos;
		gl_Position = gl_in[i].gl_Position;
        EmitVertex();

		//passToFrag.faceIndex = passFromVertex[i].faceIndex;
		//passToFrag.UvCoords = passFromVertex[i].UvCoords;
		//passToFrag.normal = passFromVertex[i].normal;
		//passToFrag.fragPos = passFromVertex[i].fragPos;
		//gl_Position = gl_in[i].gl_Position + passFromVertex[i].normal;
        //EmitVertex();
		//
		//EndPrimitive();
	}
	EndPrimitive();
}