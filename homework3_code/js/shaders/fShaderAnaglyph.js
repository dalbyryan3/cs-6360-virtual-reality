/**
 * @file Fragment shader for anaglyph rendering
 *
 * @copyright The Board of Trustees of the Leland Stanford Junior University
 * @version 2021/04/01
 */

/* TODO (2.4.3) Color Channel Multiplexing */

var shaderID = "fShaderAnaglyph";

var shader = document.createTextNode( `
/**
 * WebGL doesn't set any default precision for fragment shaders.
 * Precision for vertex shader is set to "highp" as default.
 * Do not use "lowp". Some mobile browsers don't support it.
 */

precision mediump float;

// uv coordinates after interpolation
varying vec2 textureCoords;

// Texture map for the left eye
uniform sampler2D textureMapL;

// Texture map for the right eye
uniform sampler2D textureMapR;

float computeGrayscale(vec3 rgbVec){
	return 0.2989*rgbVec.r + 0.5870*rgbVec.g + 0.1140*rgbVec.b;
}

void main() {
	gl_FragColor.r = computeGrayscale(texture2D( textureMapL,  textureCoords ).rgb);
	float gbVal = computeGrayscale(texture2D( textureMapR,  textureCoords ).rgb);
	gl_FragColor.g = gbVal;
	gl_FragColor.b = gbVal;

	// Just a test for 2.4.4
	// gl_FragColor.r = texture2D( textureMapL,  textureCoords ).r;
	// gl_FragColor.g = texture2D( textureMapR,  textureCoords ).g;
	// gl_FragColor.b = texture2D( textureMapR,  textureCoords ).b;
}
` );

var shaderNode = document.createElement( "script" );

shaderNode.id = shaderID;

shaderNode.setAttribute( "type", "x-shader/x-fragment" );

shaderNode.appendChild( shader );

document.body.appendChild( shaderNode );
