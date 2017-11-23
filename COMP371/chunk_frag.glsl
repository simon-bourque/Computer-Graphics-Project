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

out vec4 finalColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	float bias = 0.0005;
	float shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;

	return shadow;
}

vec4 FogCalculation(vec4 tempColor, float dist, vec3 viewDir)
{
	const float density = 0.008;
	float fogAmount = (1.0 - exp( -dist*density))*(1.0 - exp( -dist*density));
	float sunAmount = max( dot( viewDir,-lightDirection ), 0.0 );
    vec3  fogColor  = mix( vec3(0.588235,0.74118,0.99608),lightColor, pow(sunAmount,8.0) ); // Scatter sunlight color near sun pos
	return mix( tempColor, vec4(fogColor,1.0), fogAmount );
}

void main() {

	vec4 tempColor = texture(tilesheet, vec3(passFromVertex.UvCoords.x, passFromVertex.UvCoords.y, passFromVertex.faceIndex));

	// Should be unique per material instead of per light so these are hardcoded for now
	float ambientCoefficient = 0.2;
	float diffuseCoefficient = 0.9;
	float specularCoefficient = 0.2;
	float shininess = 1;

	//Ambient Lighting
	vec3 ambient = ambientCoefficient * lightColor;

	//Diffuse Lighting
	float diff  = diffuseCoefficient * max(dot(passFromVertex.normal, lightDirection), 0.0);
	vec3 diffuse = diff * lightColor;

	//Specular Lighting
	vec3 specular = vec3(0.0, 0.0, 0.0);
	
	// Should not add a specular component when the diffuse dot product is 0
	vec3 viewDir = normalize(viewPos - vec3(passFromVertex.fragPos));
	if (diff != 0.0) {
		vec3 reflectDir = reflect(-lightDirection, passFromVertex.normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
		specular = specularCoefficient * spec * lightColor;
	}

	float shadowFactor = ShadowCalculation(passFromVertex.fragPosLightSpace);
	tempColor = vec4((ambient + (1.0 - shadowFactor) * (diffuse + specular)), 1.0f) * tempColor;
	
	// Add Fog if blocks are far enough
	float dist = distance(viewPos, vec3(passFromVertex.fragPos));
	const float fogRenderMinDist = 100;
	if(dist > fogRenderMinDist)
		tempColor = FogCalculation(tempColor, dist - fogRenderMinDist, viewDir);

	finalColor =  clamp(tempColor, 0.0, 1.0);
}
