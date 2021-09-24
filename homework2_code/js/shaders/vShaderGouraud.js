/**
 * @file Gouraud vertex shader
 *
 * @copyright The Board of Trustees of the Leland Stanford Junior University
 * @version 2021/04/01
 */

/* TODO (2.1.2) and (2.1.3) */

var shaderID = "vShaderGouraud";

var shader = document.createTextNode( `
/**
 * varying qualifier is used for passing variables from a vertex shader
 * to a fragment shader. In the fragment shader, these variables are
 * interpolated between neighboring vertexes.
 */
varying vec3 vColor; // Color at a vertex

uniform mat4 viewMat;
uniform mat4 projectionMat;
uniform mat4 modelViewMat;
uniform mat3 normalMat;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;

uniform vec3 attenuation;

uniform vec3 ambientLightColor;

attribute vec3 position;
attribute vec3 normal;


/***
 * NUM_POINT_LIGHTS is replaced to the number of point lights by the
 * replaceNumLights() function in teapot.js before the shader is compiled.
 */
#if NUM_POINT_LIGHTS > 0

	struct PointLight {
		vec3 position;
		vec3 color;
	};

	uniform PointLight pointLights[ NUM_POINT_LIGHTS ];

#endif


void main() {
	float kc = attenuation[0];
	float kl = attenuation[1];
	float kq = attenuation[2];

	// Transform to view space
	vec3 normalVecView = normalize(normalMat * normal); // Object space to view space for vector
	vec3 vertPosView = vec3(modelViewMat * vec4(position, 1.0)); // Object space to view space for point

	// Compute ambient reflection
	vec3 ambientReflection = material.ambient * ambientLightColor;

	vec3 lightingSum = vec3(0.0);
	for (int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		// Compute diffuse reflection
		vec3 lightPosView = vec3(viewMat * vec4(pointLights[i].position, 1.0)); // World space to view space for point
		vec3 L = lightPosView - vertPosView;
		float r = length(L);
		vec3 Lhat = normalize(L);
		vec3 diffuseReflection = material.diffuse * pointLights[i].color * max(dot(Lhat,normalVecView),0.0);

		// Compute specular reflection 
		vec3 R = -reflect(Lhat, normalVecView);
		vec3 specularReflection = material.specular * pointLights[i].color * pow(max(dot(R,normalVecView),0.0), material.shininess);

		float attenuationValue = (1.0/(kc + kl*r + kq*(r*r)));

		lightingSum += attenuationValue * (diffuseReflection + specularReflection);
	}
	vColor = ambientReflection + lightingSum;

	gl_Position =
		projectionMat * modelViewMat * vec4( position, 1.0 );
}
` );

var shaderNode = document.createElement( "script" );

shaderNode.id = shaderID;

shaderNode.setAttribute( "type", "x-shader/x-vertex" );

shaderNode.appendChild( shader );

document.body.appendChild( shaderNode );
