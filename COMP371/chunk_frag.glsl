#version 430

out vec3 finalColor;

in float height;

void main() {
	float h = mod(height,10.0)/10.0;
	float h2 = mod(height,100.0)/100.0;
	finalColor = vec3(0.2, h2, h);
}
