/**
 * @file Unwarp fragment shader
 *
 * @copyright The Board of Trustees of the Leland Stanford Junior University
 * @version 2021/04/01
 */

/* TODO (2.2.2) Fragment shader implementation */

var shaderID = "fShaderUnwarp";

var shader = document.createTextNode( `
/**
 * WebGL doesn't set any default precision for fragment shaders.
 * Precision for vertex shader is set to "highp" as default.
 * Do not use "lowp". Some mobile browsers don't support it.
 */

precision mediump float;

varying vec2 textureCoords;

// texture rendered in the first rendering pass
uniform sampler2D map;

// center of lens for un-distortion
// in normalized coordinates between 0 and 1
uniform vec2 centerCoordinate;

// [width, height] size of viewport in [mm]
// viewport is the left/right half of the browser window
uniform vec2 viewportSize;

// lens distortion parameters [K_1, K_2]
uniform vec2 K;

// distance between lens and screen in [mm]
uniform float distLensScreen;

void main() {
	gl_FragColor = texture2D( map, textureCoords );

	float r_unnorm = sqrt(pow((textureCoords[0]-centerCoordinate[0])*viewportSize[0], 2.0) + pow((textureCoords[1]-centerCoordinate[1])*viewportSize[1], 2.0)); // Multiplying by viewport size changes to mm scaling
	float r_norm = r_unnorm / distLensScreen; 
	float skew = 1.0 + K[0]*pow(r_norm, 2.0) + K[1]*pow(r_norm, 4.0);
	vec2 textureCoordsDistorted = textureCoords * skew; 

	if(textureCoordsDistorted[0] < 0.0 || textureCoordsDistorted[0] >= 1.0 || textureCoordsDistorted[1] < 0.0 || textureCoordsDistorted[1] >= 1.0)
	{
		gl_FragColor.rgb = vec3(0.0, 0.0, 0.0);
	}
	else
	{
		gl_FragColor = texture2D( map, textureCoordsDistorted );
	}
}
` );


var shaderNode = document.createElement( "script" );

shaderNode.id = shaderID;

shaderNode.setAttribute( "type", "x-shader/x-fragment" );

shaderNode.appendChild( shader );

document.body.appendChild( shaderNode );
