
/**
 * @file functions to compute model/view/projection matrices
 *
 * @version 2021/04/01

 */

/**
 * MVPmat
 *
 * @class MVPmat
 * @classdesc Class for holding and computing model/view/projection matrices.
 *
 * @param  {DisplayParameters} dispParams    display parameters
 */
var MVPmat = function ( dispParams ) {

	// Alias for accessing this from a closure
	var _this = this;


	// A model matrix
	this.modelMat = new THREE.Matrix4();

	// A view matrix
	this.viewMat = new THREE.Matrix4();

	// A projection matrix
	this.projectionMat = new THREE.Matrix4();


	var topViewMat = new THREE.Matrix4().set(
		1, 0, 0, 0,
		0, 0, - 1, 0,
		0, 1, 0, - 1500,
		0, 0, 0, 1 );

	/* Functions */

	// A function to compute a model matrix based on the current state
	//
	// INPUT
	// state: state of StateController
	function computeModelTransform( state ) {

		/* (2.1.1.3) Matrix Update / (2.1.2) Model Rotation  */

		var T_trans = new THREE.Matrix4().makeTranslation(state.modelTranslation.x, state.modelTranslation.y, state.modelTranslation.z);
		var T_rotY = new THREE.Matrix4().makeRotationY((Math.PI/180)*state.modelRotation.y); // Convert degrees to radians 
		var T_rotX = new THREE.Matrix4().makeRotationX((Math.PI/180)*state.modelRotation.x); // Convert degrees to radians
		var modelTransform = T_rotX.clone().multiply(T_rotY).multiply(T_trans); // First translate, then rotation about y-axis, then rotation about x-axis last

		return modelTransform;
	}

	// A function to compute a view matrix based on the current state
	//
	// NOTE
	// Do not use lookAt().
	//
	// INPUT
	// state: state of StateController
	function computeViewTransform( state ) {

		/* (2.2.3) Implement View Transform */

		var e = state.viewerPosition;
		var p = state.viewerTarget;

		var Tv_1 = new THREE.Matrix4().set(
			1, 0, 0, -e.x,
			0, 1, 0, -e.y,
			0, 0, 1, -e.z,
			0, 0, 0, 1 );

		var g = p.clone().sub(e);
		var c_hat = g.clone().divideScalar(g.length());
		var z_hat = c_hat.clone().multiplyScalar(-1);
		var u_hat = new THREE.Vector3().set(0, 1, 0); // Up is fixed for this application
		var x_hat = u_hat.clone().cross(z_hat).normalize();
		var y_hat = z_hat.clone().cross(x_hat);
		var Tv_2 = new THREE.Matrix4().set(
			x_hat.x, x_hat.y, x_hat.z, 0,
			y_hat.x, y_hat.y, y_hat.z, 0,
			z_hat.x, z_hat.y, z_hat.z, 0,
			0, 0, 0, 1 );

		var Tv = Tv_2.clone().multiply(Tv_1);

		return Tv;
	}

	// A function to compute a perspective projection matrix based on the
	// current state
	//
	// NOTE
	// Do not use makePerspective().
	//
	// INPUT
	// Notations for the input is the same as in the class.
	function computePerspectiveTransform(
		left, right, top, bottom, clipNear, clipFar ) {

		/* (2.3.1) Implement Perspective Projection */

		var n = clipNear;
		var f = clipFar;

		var Tcan  = new THREE.Matrix4().set(
			((2*n)/(right-left)), 0, ((right+left)/(right-left)), 0,
			0, ((2*n)/(top-bottom)), ((top+bottom)/(top-bottom)), 0,
			0, 0, ((n+f)/(n-f)), ((2*f*n)/(n-f)),
			0, 0, -1, 0);

		return Tcan;
	}

	// A function to compute a orthographic projection matrix based on the
	// current state
	//
	// NOTE
	// Do not use makeOrthographic().
	//
	// INPUT
	// Notations for the input is the same as in the class.
	function computeOrthographicTransform(
		left, right, top, bottom, clipNear, clipFar ) {

		/* (2.3.2) Implement Orthographic Projection */

		var n = clipNear;
		var f = clipFar;

		var Tst  = new THREE.Matrix4().set(
			(2/(right-left)), 0, 0, (-(right+left)/(right-left)),
			0, (2/(top-bottom)), 0, (-(top+bottom)/(top-bottom)),
			0, 0, (2/(n-f)), (-(n+f)/(f-n)),
			0, 0, 0, 1);

		return Tst;
	}

	// Update the model/view/projection matrices
	// This function is called in every frame (animate() function in render.js).
	function update( state ) {

		// Compute model matrix
		this.modelMat.copy( computeModelTransform( state ) );

		// Use the hard-coded view and projection matrices for top view
		if ( state.topView ) {

			this.viewMat.copy( topViewMat );

			var right = ( dispParams.canvasWidth * dispParams.pixelPitch / 2 )
				* ( state.clipNear / dispParams.distanceScreenViewer );

			var left = - right;

			var top = ( dispParams.canvasHeight * dispParams.pixelPitch / 2 )
				* ( state.clipNear / dispParams.distanceScreenViewer );

			var bottom = - top;

			this.projectionMat.makePerspective( left, right, top, bottom, 1, 10000 );

		} else {

			// Compute view matrix
			this.viewMat.copy( computeViewTransform( state ) );

			// Compute projection matrix
			if ( state.perspectiveMat ) {

				var right = ( dispParams.canvasWidth * dispParams.pixelPitch / 2 )
				* ( state.clipNear / dispParams.distanceScreenViewer );

				var left = - right;

				var top = ( dispParams.canvasHeight * dispParams.pixelPitch / 2 )
				* ( state.clipNear / dispParams.distanceScreenViewer );

				var bottom = - top;

				this.projectionMat.copy( computePerspectiveTransform(
					left, right, top, bottom, state.clipNear, state.clipFar ) );

			} else {

				var right = dispParams.canvasWidth * dispParams.pixelPitch / 2;

				var left = - right;

				var top = dispParams.canvasHeight * dispParams.pixelPitch / 2;

				var bottom = - top;

				this.projectionMat.copy( computeOrthographicTransform(
					left, right, top, bottom, state.clipNear, state.clipFar ) );

			}

		}

	}



	/* Expose as public functions */

	this.computeModelTransform = computeModelTransform;

	this.computeViewTransform = computeViewTransform;

	this.computePerspectiveTransform = computePerspectiveTransform;

	this.computeOrthographicTransform = computeOrthographicTransform;

	this.update = update;

};
