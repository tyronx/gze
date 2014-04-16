/* Stuck() function that actually works 
 * 
 * The original Stuck() doesn't test if there is solid material below each vertex, thus the object can fall down
 */

#strict

global func Stuck(object obj) {
	if (!obj) obj = this();
	if (!obj) return(0); 
 
	for (var i = 0; i < GetVertexNum(obj); i++) {
		// The second check if there is solid below the vertex is what is actually missing in the native Stuck()
		if (obj->GBackSolid(GetVertex(i, 0, obj), GetVertex(i, 1, obj)) && obj->GBackSolid(GetVertex(i, 0, obj), GetVertex(i, 1, obj)+1)) {
			return(1);
		}
	}
	return(0);
}