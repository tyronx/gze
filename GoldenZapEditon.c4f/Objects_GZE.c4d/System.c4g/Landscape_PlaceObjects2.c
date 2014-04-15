#strict

/* 
 * Better EarthObj Placement. Besides many new parameters it also does the following:
 * - Leaves at least 10 pixels distance between each placed object
 * - Makes sure they are *really* stuck in the material
 * Parameters:
 *
 * objectid:  Object to be placed
 * quantity: Quantity to be planted
 * rect: Target rect as array [x,y,wdt,hgt], Offset in local calls
 * materials: array of materials it should be placed in. If not set, the objects will be placed in Materials with Soil=1
 * nearbymaterials: material the object has to be somewhat close to
 *
 * Returns:
 * Amount of actually placed objects (algo gives up after 10.000 iterations) 
 *
 * Examples:
 *
 * PlaceObjects2(FLNT, 20, 0, LandscapeHeight()/3, LandscapeWidth(), LandscapeHeight()/3, ["Rock", "Granite"])
 * - Places 20 flints inside rock and granite between 33% and 66% landscape height 
 */
global func PlaceObjects2(id objectid, int quantity, array rect, array inmaterials, int nearbymaterial) {
	var x = rect[0], y = rect[1], wdt = rect[2], hgt = rect[3];
	
	var haveinmaterials = GetType(inmaterials) == C4V_Array();
	
	
	// Failsafe
	var attempts = 0;
	var placed = 0;
	
	// Create a dozen random places and try there
	var rndx, rndy, valid, obj, matatpos;
	var objhgt = GetDefHeight(objectid);
	
		
	if (nearbymaterial) {
		while (quantity > 0 && attempts++ < 10000) {
			rndx = x + Random(wdt);
			rndy = y + Random(hgt);
			
			matatpos = GetMaterial(rndx,rndy);
	
			if (matatpos == nearbymaterial) {
				for (var i = 0; i < 8; i++) {
					// Generate a random number between -10 to -4 or 4 to 10
					rndx += RandomX(10, 20) * (Random(2)*2-1);
					rndy += RandomX(10, 20) * (Random(2)*2-1);
					matatpos = GetMaterial(rndx,rndy);
					
					valid = false;
					if (haveinmaterials) {
						for (var mat in inmaterials) {
							if (mat == matatpos && mat != nearbymaterial) { 
								valid = true;
								break;
							}
						}
					} else {
						valid = isMaterialSoil(rndx, rndy);
					}
					
					if (valid) {
						obj = CreateObject(objectid, rndx, rndy + objhgt / 2, -1);
						obj->SetR(Random(360));
					}

					if (Stuck(obj)) {
						placed++;
						quantity--;
						break;
					} else {
						RemoveObject(obj);
					}
				}
			}
		}
	} else {
	
		
		while (quantity > 0 && attempts++ < 10000) {
			rndx = x + Random(wdt);
			rndy = y + Random(hgt);
			
			matatpos = GetMaterial(rndx,rndy);
			
			valid = false;
			
			if (haveinmaterials) {
				for (var mat in inmaterials) {
					if (mat == matatpos) { 
						valid = true;
						break;
					}
				}
			} else {
				valid = isMaterialSoil(rndx, rndy);
			}
			
			if (valid) {
				obj = CreateObject(objectid, rndx, rndy + objhgt / 2, -1);
				obj->SetR(Random(360));
			}
			
			if (Stuck(obj)) {
				placed++;
				quantity--;
			} else {
				RemoveObject(obj);
			}
		}
	}
	return placed;
}
