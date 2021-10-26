/**
 * @file Fragment shader for foveated rendering
 *
 * @copyright The Board of Trustees of the Leland Stanford Junior University
 * @version 2021/04/01
 */

/* TODO (2.2.4) Fragment Shader Foveation Blur */

var shaderID = "fShaderFoveated";

var shader = document.createTextNode( `
/***
 * WebGL doesn't set any default precision for fragment shaders.
 * Precision for vertex shader is set to "highp" as default.
 * Do not use "lowp". Some mobile browsers don't support it.
 */
precision mediump float;

// texture or uv coordinates of current fragment in normalized coordinates [0,1]
varying vec2 textureCoords;

// texture map from the first rendering pass
uniform sampler2D textureMap;

// resolution of the window in [pixels]
uniform vec2 windowSize; // width, height 

// window space coordinates of gaze position in [pixels]
uniform vec2 gazePosition;

// eccentricity angle at boundary of foveal and middle layers
uniform float e1;

// eccentricity angle at boundary of middle and outer layers
uniform float e2;

// visual angle of one pixel
uniform float pixelVA;

// radius of middle layer blur kernel [in pixels]
const float middleKernelRad = 2.0;

// radius of outer layer blur kernel [in pixels]
const float outerKernelRad = 4.0;

// gaussian blur kernel for middle layer (5x5)
uniform float middleBlurKernel[int(middleKernelRad)*2+1];

// gaussian blur kernel for outer layer (9x9)
uniform float outerBlurKernel[int(outerKernelRad)*2+1];

void main() {

	gl_FragColor = texture2D( textureMap,  textureCoords );

	// Calculate e of fragment 
	float e = distance(gazePosition, textureCoords*windowSize) * pixelVA;

	if (e > e2)
	{
		gl_FragColor.rgb=vec3(0);
		for (int i=-int(outerKernelRad); i<=int(outerKernelRad); i++)
		{
			for (int j=-int(outerKernelRad); j<=int(outerKernelRad); j++)
			{
				// Can make 2D gaussian kernel out of kernel array outer product 
				gl_FragColor.rgb += (outerBlurKernel[i+int(outerKernelRad)]*outerBlurKernel[j+int(outerKernelRad)]) * texture2D(textureMap, vec2(textureCoords.x + float(i)/windowSize.x, textureCoords.y + float(j)/windowSize.y)).rgb; 
			}
		}
	}
	else if (e > e1)
	{
		gl_FragColor.rgb=vec3(0);
		for (int i=-int(middleKernelRad); i<=int(middleKernelRad); i++)
		{
			for (int j=-int(middleKernelRad); j<=int(middleKernelRad); j++)
			{
				// Can make 2D gaussian kernel out of kernel array outer product 
				gl_FragColor.rgb += (middleBlurKernel[i+int(middleKernelRad)]*middleBlurKernel[j+int(middleKernelRad)]) * texture2D(textureMap, vec2(textureCoords.x + float(i)/windowSize.x, textureCoords.y + float(j)/windowSize.y)).rgb; 
			}
		}
	}
	// Otherwise won't blur
}
` );

var shaderNode = document.createElement( "script" );

shaderNode.id = shaderID;

shaderNode.setAttribute( "type", "x-shader/x-fragment" );

shaderNode.appendChild( shader );

document.body.appendChild( shaderNode );
