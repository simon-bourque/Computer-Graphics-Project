#version 430

layout(binding = 0) uniform sampler2DArray tilesheet;
layout(binding = 1) uniform sampler2D shadowMap;

in VertexData {
	flat int faceIndex;
	vec2 UvCoords;
	vec3 normal;
	vec3 fragPos;
	vec4 fragPosLightSpace;
} passFromVertex;

uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 lightDirection;
uniform vec3 lightPos;
uniform float ambientStrength;
uniform float specularStrength;

out vec4 finalColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

	return shadow;
}

void main() {

	vec4 tempColor = texture(tilesheet, vec3(passFromVertex.UvCoords.x, passFromVertex.UvCoords.y, passFromVertex.faceIndex));

	//Ambient Lighting
	vec3 ambient = ambientStrength * lightColor;

	//Diffuse Lighting
	float diff = max(dot(passFromVertex.normal, lightDirection), 0.0);
	vec3 diffuse = diff * lightColor;

	//Specular Lighting
	vec3 viewDir = normalize(viewPos - vec3(passFromVertex.fragPos));
	vec3 reflectDir = reflect(-lightDirection, passFromVertex.normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2); //CHANGE 2 TO A VARIABLE SHININESS COEFFICIENT DEPENDING ON BLOCK TYPE
	vec3 specular = specularStrength * spec * lightColor;

	float shadowFactor = ShadowCalculation(passFromVertex.fragPosLightSpace);
	tempColor = vec4((ambient + (1.0 - shadowFactor) * (diffuse + specular)), 1.0f) * tempColor;

	finalColor = tempColor;
}
