#strict 2

/* Functions for probing properties of the world surface. Helpful when placing objects near of above surface borders
 * 
 * All functions require a stepping parameter which determines the accuracy of the probing 
 * stepping = 1   => checks every pixel, so 100% accurate
 * stepping = 10    => checks every 10th pixel, much less accurate but much faster
 *
 *
 * All Functions use global coordinates
 */

/* Moves vertically up or down (default) the landscape to find a solid pixel */
// always global coordinates, stepping parameter is optional
// e.g. GetYSolid(LandscapeWidth()/2) - finds the Y Coordinate of the first solid pixel in the middle of the landscape
global func GetYSolid(int x, int y, int stepping) {
	if (!stepping) stepping = 1;
	while (!GBackSolid(x - GetX(),y - GetY())) {
		y+=stepping;
		if (y>=LandscapeHeight() || y <= 0) {
			return (-1);
		}
	}
	return(y);
}

/* Moves vertically up (default) or down the landscape to find a non solid pixel */
// always global coordinates, stepping parameter is optional
global func GetYFree(int x, int y, int stepping) {
	if (!stepping) stepping = -1;
	while (GBackSolid(x - GetX(),y - GetY())) {
		y+=stepping;
		if (y>=LandscapeHeight() || y <= 0) {
			return(-1);
		}
	}
	return(y);
}


/* Moves vertically up or down to find the bordering y-position between solid and nonsolid */
// This can help you figure out the angle of a surface if you sample 2 spots
global func GetYBorder(int x, int y, int stepping) {
	if (GBackSolid(x - GetX(), y - GetY())) {
		return GetYFree(x, y, -stepping);
	} else {
		return GetYSolid(x, y, stepping);
	}
}


// Returns the angle of the surface, probed at the points -radius and +radius
// 90 degree == completely even surface
global func GetSurfaceAngle(int x, int y, int stepping, int radius) {
	var left = GetYBorder(x - radius, y, stepping);
	var right = GetYBorder(x + radius, y, stepping);

	return Angle(x - radius, left, x+radius, right);
}