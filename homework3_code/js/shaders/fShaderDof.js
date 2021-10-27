/**
 * @file Fragment shader for DoF rendering
 *
 * @copyright The Board of Trustees of the Leland Stanford Junior University
 * @version 2021/04/01
 */

/* TODO (2.3) DoF Rendering */

var shaderID = "fShaderDof";

var shader = document.createTextNode( `
/**
 * WebGL doesn't set any default precision for fragment shaders.
 * Precision for vertex shader is set to "highp" as default.
 * Do not use "lowp". Some mobile browsers don't support it.
 */
precision mediump float;

// uv coordinates after interpolation
varying vec2 textureCoords;

// texture map from the first rendering
uniform sampler2D textureMap;

// depth map from the first rendering
uniform sampler2D depthMap;

// Projection matrix used for the first pass
uniform mat4 projectionMat;

// Inverse of projectionMat
uniform mat4 invProjectionMat;

// resolution of the window in [pixels]
uniform vec2 windowSize;

// Gaze position in [pixels]
uniform vec2 gazePosition;

// Diameter of pupil in [mm]
uniform float pupilDiameter;

// pixel pitch in [mm]
uniform float pixelPitch;

const float searchRad = 11.0;

vec3 windowToViewSpace( vec2 p ) {
	vec2 xyNDC = (p - (windowSize*0.5)) / (windowSize*0.5);
	float zNDC = 2.0*(texture2D(depthMap, p/windowSize).r) - 1.0;
	vec3 vNDC = vec3(xyNDC, zNDC);
	float wClip = projectionMat[3][2] / (zNDC - projectionMat[2][2]);
	vec4 vClip = vec4(wClip*vNDC, wClip);
	vec4 vView = invProjectionMat * vClip;
	vView = vView * (1.0/vView[3]); // homogenize
	return vec3(vView[0], vView[1], vView[2]);
}
// Compute the distance to fragment in [mm]
// p: texture coordinate of a fragment / a gaze position
//
// Note: GLSL is column major
float distToFrag( vec2 p ) {

	/* TODO (2.3.1) Distance to Fragment */

	// return distance(windowToViewSpace(textureCoords), windowToViewSpace(p));
	return length(windowToViewSpace(p)); // Distance from eye in view space to p is length of point in view space
}
// compute the circle of confusion in [mm]
// fragDist: distance to current fragment in [mm]
// focusDist: distance to focus plane in [mm]
float computeCoC( float fragDist, float focusDist ) {

	/* TODO (2.3.2) Circle of Confusion Computation */

	return pupilDiameter * abs(focusDist-fragDist) / focusDist;
}
// compute depth of field blur and return color at current fragment
vec3 computeBlur() {

	/* TODO (2.3.3) Retinal Blur */

	float blurRadius = computeCoC(distToFrag(textureCoords*windowSize), distToFrag(gazePosition)) / pixelPitch;

	vec3 final_color = vec3(0);
	float n = 0.0;
	for (int i = -int(searchRad); i <= int(searchRad); i++)
	{
		for (int j = -int(searchRad); j <= int(searchRad); j++)
		{
			if(float(i*i+j*j) <= float(int(blurRadius)*int(blurRadius)))
			{
				final_color += texture2D(textureMap, vec2(textureCoords.x + float(i)/windowSize.x, textureCoords.y + float(j)/windowSize.y)).rgb;
				n = n + 1.0;
			}
		}
	}
	return final_color * (1.0 / n);
	// Visualize blur radius
	// return vec3(blurRadius/10.0);
}


void main() {

	gl_FragColor = texture2D( textureMap,  textureCoords );
	gl_FragColor.rgb = computeBlur();

}
` );


var shaderNode = document.createElement( "script" );

shaderNode.id = shaderID;

shaderNode.setAttribute( "type", "x-shader/x-fragment" );

shaderNode.appendChild( shader );

document.body.appendChild( shaderNode );
