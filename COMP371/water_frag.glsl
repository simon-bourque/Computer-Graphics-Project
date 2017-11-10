#version 430

layout(binding = 0) uniform sampler2D normalMap;
layout(binding = 1) uniform sampler2D normalMap2;
layout(binding = 2) uniform sampler2D dudvMap;
layout(binding = 3) uniform sampler2D refractionTexture;

in vec2 tiledNormalUvCoords;
in vec2 tiledDuDvCoords;
in vec3 originalPos;
in vec4 clipSpacePos;

uniform vec3 viewPos;

out vec4 finalColor;

uniform float panner;

// Light uniforms
uniform vec3 lightDirection;
uniform vec3 lightColor;

void main() {

	// Water color rgb(68, 103, 125)
	vec4 tempColor = vec4(0.26666666666, 0.40392156862, 0.49019607843, 1.0);
	
	// Pan and sample normal maps
	vec3 normal1 = ((texture(normalMap, tiledNormalUvCoords + vec2(panner,panner)).rgb) * 2) - 1;
	vec3 normal2 = ((texture(normalMap2, tiledNormalUvCoords + vec2(-panner,-panner)).rgb) * 2) - 1;

	// Convert normals from tangent space to world space
	// This can be greatly simplified since we are rendering on a plane
	normal1 = normal1.xzy;
	normal2 = normal2.xzy;
	
	// Mix the normal maps together
	vec3 normal = normalize(normal1 + normal2);


	// Should be unique per material instead of per light so these are hardcoded for now
	float ambientCoefficient = 0.5;
	float diffuseCoefficient = 0.7;
	float specularCoefficient = 3;
	float shininess = 25;

	//Ambient Lighting
	vec3 ambient = ambientCoefficient * lightColor;

	//Diffuse Lighting
	float diff  = diffuseCoefficient * max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = diff * lightColor;

	//Specular Lighting
	vec3 specular = vec3(0.0, 0.0, 0.0);
	
	// Should not add a specular component when the diffuse dot product is 0
	if (diff != 0.0) {
		vec3 viewDir = normalize(viewPos - vec3(originalPos));
		vec3 reflectDir = reflect(-lightDirection, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
		specular = specularCoefficient * spec * lightColor;
	}

	tempColor = vec4((ambient + diffuse + specular), 1.0) * tempColor;

	// Convert clipSpacePosition to UV
	vec2 screenPos = clipSpacePos.xy / clipSpacePos.w;
	vec2 uv = vec2((screenPos.x / 2.0) + 0.5, (screenPos.y / 2.0) + 0.5); // Convert OpenGL screen space to uv space
	vec2 dudvDisp = ((texture(dudvMap, tiledDuDvCoords + vec2(panner,panner)).rg * 2.0) - 1.0) * 0.007; // Adjust dudv vectors to range from -1.0 to 1.0 and reduce strength
	
	
	// Mix everything together
	finalColor = clamp(mix(tempColor, texture(refractionTexture, uv + dudvDisp), 0.5), 0.0,1.0);
	
	//finalColor = vec4(normal, 1.0);
	//finalColor = vec4(specular,1.0);
	//finalColor = vec4(diffuse, 1.0);
	//finalColor = vec4(ambient, 1.0);
	
}