#strict

// See official DrawMaterialQuad() for more documentation

// Draws a single pixel at given coordinate
// offset when called locally
global func DrawMaterialPixel(string sMaterial, int iX, int iY, bool sub) {
	return DrawMaterialQuad2(sMaterial, iX, iY, iX+1, iY, iX+1, iY+1, iX, iY+1, sub);
}

// Draws a 1 pixel wide line of material between 2 points
// offset when called locally
global func DrawMaterialLine(string sMaterial, int iX1, int iY1, int iX2, int iY2, bool sub) {
	return DrawMaterialQuad2(sMaterial, iX1, iY1, iX1+1, iY1, iX2, iY2, iX2+1, iY2, sub);
}

// same as DrawMaterialQuad2() but offset when called locally
global func DrawMaterialQuad2(string sMaterial, int iX1, int iY1, int iX2, int iY2, int iX3, int iY3, int iX4, int iY4, bool sub) {
	return DrawMaterialQuad(sMaterial, GetX()+iX1, GetY()+iY1, GetX()+iX2, GetY()+iY2, GetX()+iX3, GetY()+iY3, GetX()+iX4, GetY()+iY4, sub);
}
