#version 330 core

uniform sampler2D 	diffuseTex;
uniform vec3		cameraPos;
uniform vec4		lightColour;
uniform vec3		lightPos;
uniform float		lightRadius;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void) {

	fragColour = texture(diffuseTex, IN.texCoord);

//	vec3 incident	= normalize(lightPos);
//	vec3 viewDir	= normalize(cameraPos - IN.worldPos);
//	vec3 halfDir	= normalize(incident + viewDir);
//
//	vec4 diffuse = texture(diffuseTex, IN.texCoord);
//
//	float lambert = max(dot(incident, IN.normal), 0.0f);
//	float specFactor = clamp(dot(halfDir, IN.normal),0.0,1.0);
//	specFactor = pow(specFactor,60.0);
//
//	vec3 surface = (diffuse.rgb * lightColour.rgb);
//	fragColour.rgb = surface * lambert;
//	fragColour.rgb += (lightColour.rgb * specFactor)*0.1;
//	fragColour.rgb += surface * 0.1f;
	//fragColour.a = diffuse.a;
}