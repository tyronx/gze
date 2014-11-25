/* Stuck() function that actually works 
 * 
 * The original Stuck() doesn't test if there is solid material below each vertex, thus the object can fall down
 * Also there has to be material one pixel left and right to it
 */

#strict 2

global func Stuck(object obj) {
	if (!obj) obj = this;
	if (!obj) return 0; 
 
	var x,y;
	for (var i = 0; i < GetVertexNum(obj); i++) {
		x = GetVertex(i, 0, obj);
		y = GetVertex(i, 1, obj);
		
		if (obj->GBackSolid(x, y+1) && obj->GBackSolid(x-1, y) && obj->GBackSolid(x+1, y)) {
			return 1;
		}
	}
	return 0;
}