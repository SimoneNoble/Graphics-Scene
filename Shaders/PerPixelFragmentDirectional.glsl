#version 330 core

uniform sampler2D	diffuseTex;
uniform sampler2D 	diffuseTex2;

uniform vec3		cameraPos;

uniform vec4		lightColour;
uniform vec3		lightPos;
uniform float		lightRadius;

uniform vec4		lightColour2;
uniform vec3		lightPos2;
uniform float		lightRadius2;

uniform vec4		lightColour3;
uniform vec3		lightPos3;
uniform float		lightRadius3;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
	float y_height;
} IN;

out vec4 fragColour;

void main(void) {

	{
	vec3 incident	= normalize(lightPos);
	vec3 viewDir	= normalize(cameraPos - IN.worldPos);
	vec3 halfDir	= normalize(incident + viewDir);

	vec4 diffuse = texture(diffuseTex, IN.texCoord);

	if(IN.y_height >= 140) {
		diffuse = texture(diffuseTex2, IN.texCoord);
	}

	else if (IN.y_height <= 140 && IN.y_height > 120) {
		vec4 tex1 = texture (diffuseTex, IN.texCoord);
		vec4 tex2 = texture (diffuseTex2, IN.texCoord);
		float blendFactor = ((IN.y_height - 100) / (120));
		diffuse = mix(tex1,tex2,blendFactor);
	}	

	float lambert = max(dot(incident, IN.normal), 0.0f);
	float specFactor = clamp(dot(halfDir, IN.normal),0.0,1.0);
	specFactor = pow(specFactor,60.0);

	vec3 surface = (diffuse.rgb * lightColour.rgb);
	fragColour.rgb = surface * lambert;
	fragColour.rgb += (lightColour.rgb * specFactor)*0.1;
	fragColour.rgb += surface * 0.1f;
	//fragColour.a = diffuse.a;
	}

	{
	vec3 incident	= normalize(lightPos2);
	vec3 viewDir	= normalize(cameraPos - IN.worldPos);
	vec3 halfDir	= normalize(incident + viewDir);

	vec4 diffuse = texture(diffuseTex, IN.texCoord);

	if(IN.y_height >= 140) {
		diffuse = texture(diffuseTex2, IN.texCoord);
	}

	else if (IN.y_height <= 140 && IN.y_height > 120) {
		vec4 tex1 = texture (diffuseTex, IN.texCoord);
		vec4 tex2 = texture (diffuseTex2, IN.texCoord);
		float blendFactor = ((IN.y_height - 100) / (120));
		diffuse = mix(tex1,tex2,blendFactor);
	}	

	float lambert = max(dot(incident, IN.normal), 0.0f);

	float specFactor = clamp(dot(halfDir, IN.normal),0.0,1.0);
	specFactor = pow(specFactor,60.0);

	vec3 surface = (diffuse.rgb * lightColour2.rgb);
	fragColour.rgb += surface * lambert;
	fragColour.rgb += (lightColour2.rgb * specFactor)*0.1;
	fragColour.rgb += surface * 0.1f;
	fragColour.a = diffuse.a;
	}

	{
	vec3 incident	= normalize(lightPos3);
	vec3 viewDir	= normalize(cameraPos - IN.worldPos);
	vec3 halfDir	= normalize(incident + viewDir);

	vec4 diffuse = texture(diffuseTex, IN.texCoord);

	if(IN.y_height >= 140) {
		diffuse = texture(diffuseTex2, IN.texCoord);
	}

	else if (IN.y_height <= 140 && IN.y_height > 120) {
		vec4 tex1 = texture (diffuseTex, IN.texCoord);
		vec4 tex2 = texture (diffuseTex2, IN.texCoord);
		float blendFactor = ((IN.y_height - 100) / (120));
		diffuse = mix(tex1,tex2,blendFactor);
	}	

	float lambert = max(dot(incident, IN.normal), 0.0f);

	float specFactor = clamp(dot(halfDir, IN.normal),0.0,1.0);
	specFactor = pow(specFactor,60.0);

	vec3 surface = (diffuse.rgb * lightColour3.rgb);
	fragColour.rgb += surface * lambert;
	fragColour.rgb += (lightColour3.rgb * specFactor)*0.1;
	fragColour.rgb += surface * 0.1f;
	fragColour.a = diffuse.a;
	}

}