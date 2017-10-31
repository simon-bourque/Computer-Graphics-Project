#version 430

layout(binding = 0) uniform sampler2DArray tilesheet;

//flat in int passFaceIndex;
//in vec2 passUvCoords;
//in vec3 passNormal;
//in vec3 fragPos;

in fragData {
	flat int faceIndex;
	vec2 UvCoords;
	vec3 normal;
	vec3 fragPos;
} passFromGeo;

uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightDirection;
uniform float ambientStrength;
uniform float specularStrength;

out vec4 finalColor;

void main() {
	//finalColor = vec4(0.0, 0.0, 1.0, 1.0);
	vec4 tempColor = texture(tilesheet, vec3(passFromGeo.UvCoords.x, passFromGeo.UvCoords.y, passFromGeo.faceIndex));

	//Ambient Lighting
	vec3 ambient = ambientStrength * lightColor;

	//Diffuse Lighting
	float diff = max(dot(passFromGeo.normal, lightDirection), 0.0);
	vec3 diffuse = diff * lightColor;

	//Specular Lighting
	vec3 viewDir = normalize(viewPos - vec3(passFromGeo.fragPos));
	vec3 reflectDir = reflect(-lightDirection, passFromGeo.normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2); //CHANGE 2 TO A VARIABLE SHININESS COEFFICIENT DEPENDING ON BLOCK TYPE
	vec3 specular = specularStrength * spec * lightColor;

	tempColor = vec4((ambient + diffuse + specular), 1.0f) * tempColor;

	finalColor = tempColor;
}
