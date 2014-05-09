#strict 2

// Useful function for placing objects on surfaces

/* Moves vertically up or down (default) the landscape to find a solid pixel */
// stepping parameter is optional
// e.g. GetYSolid(LandscapeWidth()/2) - finds the Y Coordinate of the first solid pixel in the middle of the landscape
global func GetYSolid(int x, int y, int stepping) {
	if (!stepping) stepping = 1;
	while (!GBackSolid(x,y)) {
		y+=stepping;
		if (y>=LandscapeHeight() || y <= 0) {
			return (-1);
		}
	}
	return(y);
}

/* Moves vertically up (default) or down the landscape to find a non solid pixel */
// stepping parameter is optional
global func GetYFree(int x, int y, int stepping) {
	if (!stepping) stepping = -1;
	while (GBackSolid(x,y)) {
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
	if (GBackSolid(x,y)) {
		return GetYFree(x, y, -stepping);
	} else {
		return GetYSolid(x, y, stepping);
	}
}