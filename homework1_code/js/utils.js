/**
 * @file utility functions
 * @version 2021/04/01
 */

// Cardinal red
const cardinalColor = "rgb( 140, 21, 21 )";

function vector3ToString( v ) {

	return "(" + v.x.toFixed( 1 ).toString()
		+ "," + v.y.toFixed( 1 ).toString()
		+ "," + v.z.toFixed( 1 ).toString() + ")";

}

function vector2ToString( v ) {

	return "(" + v.x.toFixed( 1 ).toString()
		+ "," + v.y.toFixed( 1 ).toString() + ")";

}
