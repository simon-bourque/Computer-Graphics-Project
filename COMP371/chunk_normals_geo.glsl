#version 430

layout(points) in;
layout(line_strip, max_vertices = 6) out;

uniform mat4 vpMatrix;

in VertexData {
	flat int faceIndex;
	vec2 UvCoords;
	vec3 normal;
	vec3 fragPos;
} passFromVertex[];

out vec4 normalColor;

void main() {
	for (int i = 0; i < gl_in.length(); i++) {
		vec3 normal = passFromVertex[i].normal;

		if (normal == vec3(1.0,0.0,0.0) || normal == vec3(-1.0,0.0,0.0)) {
			normalColor = vec4(1.0,0.0,0.0,1.0);
		}
		else if (normal == vec3(0.0,1.0,0.0) || normal == vec3(0.0,-1.0,0.0)) {
			normalColor = vec4(0.0,1.0,0.0,1.0);
		}
		else if (normal == vec3(0.0,0.0,1.0) || normal == vec3(0.0,0.0,-1.0)) {
			normalColor = vec4(0.0,0.0,1.0,1.0);
		}
		else {
			normalColor = vec4(1.0,1.0,1.0,1.0);
		}


		gl_Position = gl_in[i].gl_Position;
        EmitVertex();
		
		gl_Position = vpMatrix * vec4(passFromVertex[i].fragPos + (normal * 0.3), 1.0);
        EmitVertex();

		EndPrimitive();
	}
}