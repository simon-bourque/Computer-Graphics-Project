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

void main() {
	//finalColor = texture(normal, passUvCoords);
	//135,206,250
	vec4 tempColor = vec4(135/255.0,206/255.0,250/255.0,1);
	//vec4 tempColor = vec4(0.0,0.0,0.2,0.4);
	vec3 normal1 = texture(normalMap, tiledNormalUvCoords + vec2(panner,panner)).rgb;
	vec3 normal2 = texture(normalMap2, tiledNormalUvCoords + vec2(-panner,-panner)).rgb;
	vec3 normal = normalize(normal1 + normal2);
	//vec3 normal = mix(normal1, normal2, 0.5);

	// ##########################
	vec3 lightDirection = vec3(-0.80, -0.5, 0.0);
	vec3 lightColor = vec3(1.0,1.0,1.0);
	float ambientStrength = 0.5;
	float specularStrength = 1;
	// ##########################

	//Ambient Lighting
	vec3 ambient = ambientStrength * lightColor;

	//Diffuse Lighting
	float diff = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = diff * lightColor;

	//Specular Lighting
	vec3 viewDir = normalize(viewPos - vec3(originalPos));
	vec3 reflectDir = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2); //CHANGE 2 TO A VARIABLE SHININESS COEFFICIENT DEPENDING ON BLOCK TYPE
	vec3 specular = specularStrength * spec * lightColor;

	tempColor = vec4((ambient + diffuse + specular), 1.0f) * tempColor;

	// Convert clipSpacePosition to UV
	vec2 screenPos = clipSpacePos.xy / clipSpacePos.w;
	vec2 uv = vec2((screenPos.x / 2.0) + 0.5, (screenPos.y / 2.0) + 0.5);
	vec2 dudvDisp = ((texture(dudvMap, tiledDuDvCoords + vec2(panner,panner)).rg * 2.0) - 1.0) * 0.007;
	
	
	finalColor = mix(tempColor, texture(refractionTexture, uv + dudvDisp), 0.5);
	
	//finalColor = texture(dudvMap, tiledDuDvCoords + vec2(panner,panner));
	//finalColor = vec4(normal, 1.0);
	//finalColor = tempColor;
}