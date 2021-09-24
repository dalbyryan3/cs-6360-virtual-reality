/**
 * @file Phong fragment shader with point and directional lights
 *
 * @copyright The Board of Trustees of the Leland Stanford Junior University
 * @version 2021/04/01
 */

/* TODO (2.3) */

var shaderID = "fShaderMultiPhong";

var shader = document.createTextNode( `
/**
 * WebGL doesn't set any default precision for fragment shaders.
 * Precision for vertex shader is set to "highp" as default.
 * Do not use "lowp". Some mobile browsers don't support it.
 */
precision mediump float;

varying vec3 normalCam; // Normal in view coordinate
varying vec3 fragPosCam; // Fragment position in view cooridnate

uniform mat4 viewMat;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

uniform Material material;

uniform vec3 attenuation;

uniform vec3 ambientLightColor;

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

/***
 * NUM_DIR_LIGHTS is replaced to the number of directional lights by the
 * replaceNumLights() function in teapot.js before the shader is compiled.
 */
#if NUM_DIR_LIGHTS > 0

	struct DirectionalLight {
		vec3 direction;
		vec3 color;
	};

	uniform DirectionalLight directionalLights[ NUM_DIR_LIGHTS ];

#endif

void main() {
	float kc = attenuation[0];
	float kl = attenuation[1];
	float kq = attenuation[2];

	// Transform to view space
	vec3 normalVecView = normalize(normalCam); 
	vec3 vertPosView = fragPosCam; 

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
	
	vec3 directionalLightingSum = vec3(0.0);
	for (int i = 0; i < NUM_DIR_LIGHTS; i++)
	{
		vec3 d = -normalize(vec3(viewMat * vec4(directionalLights[i].direction, 1.0))); // World space to view space for vector

		// Compute directional diffuse reflection
		vec3 diffuseReflection = material.diffuse * directionalLights[i].color * max(dot(d,normalVecView),0.0);

		// Compute specular reflection 
		vec3 R = -reflect(d, normalVecView);
		vec3 specularReflection = material.specular * directionalLights[i].color * pow(max(dot(R,normalVecView),0.0), material.shininess);

		directionalLightingSum +=  (diffuseReflection + specularReflection);
	}

	vec3 fColor = ambientReflection + lightingSum + directionalLightingSum;

	gl_FragColor = vec4( fColor, 1.0 );

}
` );

var shaderNode = document.createElement( "script" );

shaderNode.id = shaderID;

shaderNode.setAttribute( "type", "x-shader/x-fragment" );

shaderNode.appendChild( shader );

document.body.appendChild( shaderNode );
