#strict 2

global func GetMatSoil(iMaterial) {
	return(GetMaterialVal("Soil", "Material", iMaterial));
}

/* 
 * Better Vegetation Placement. Besides many new parameters it also does the following:
 * - Makes a definition call in the object to GetVegetationRootDepth() to determine how deep the plant shall be rooted (default 5)
 * - Checks that 80% vertical space is free for the plant, ignores horizontal space
 * - Leaves at least 10 pixels distance between each placed object
 * - Can be used for any objects, e.g. chests in tunnels ;-)
 * Parameters:
 *
 * objectid:  Object to be planted
 * quantity: Quantity to be planted
 * rect: Target rect as array [x,y,wdt,hgt], Offset in local calls
 * materialsoil: the soil material in which the plant grows, use 0 for placement in Materials with Soil=1
 * underground: If 1, object will be only placed in Tunnels
 * liquid: 0 for non-submerged plants, the material number for placement in that liquid (e.g. Material("Lava") for lava plants)
 * hanging: Set to 1 for hanging plants, like stalactices hanging from caves
 * cons: if array, defines the min and max growth rate the vegetation can vary in e.g. [0,100] for random growth
 * autorotates: if array, defines the min and max rotation adjustment to the landscape in e.g. [-30,30] for -30 to +30 degree
 *
 * Returns:
 * Amount of actually placed vegetation (algo gives up after 50.000 iterations) 
 *
 * Examples:
 *
 * PlaceVegetation2(VINE, 20, [0, 0, LandscapeWidth(), LandscapeHeight()], Material("Earth"), 0, Material("Water"), 1)
 * - Places 20 hanging vines inside underground water sources
 */
global func PlaceVegetation2(id objectid, int quantity, array rect, int materialsoil, bool underground, int liquid, bool hanging, array cons, array autorotates) {
	Log("PlaceVegetation2 called for objid %v", objectid);
	var yDirection = -1;
	if (hanging) yDirection = 1;
	
	var x = rect[0], y = rect[1], wdt = rect[2], hgt = rect[3];
	var havecons = GetType(cons) == C4V_Array;
	if (!havecons) {
		cons = [0,0];
	}
	var haveautorotates = GetType(autorotates) == C4V_Array;
	
	var vegetationRootDepth = DefinitionCall(objectid, "GetVegetationRootDepth");
	if (!vegetationRootDepth) vegetationRootDepth = 5;
		
	// Failsafe
	var attempts = 0;
	var placed = 0;
	
	// At least 80% of the objects height must be free vertically
	var minverticalspace = (GetDefHeight(objectid) * 4) / 5;

	// The Material the object will be placed in
	var freemat = -1;
	if (underground) freemat = Material("Tunnel");
	if (liquid) freemat = liquid;

	// Create a dozen random places and try there
	var rndx, rndy, realy, valid, materialatpos, obj, con;
	
	// to make the loop a bit more efficient we do this calculation outside the loop - these are adjustments for root depth etc.
	var rndy_diff, realy_diff;
	if (hanging) {
		rndy_diff = GetDefHeight(objectid) - vegetationRootDepth;
		realy_diff = GetDefHeight(objectid);
	} else {
		rndy_diff = vegetationRootDepth;
		realy_diff = GetDefHeight(objectid) / 2;
	}
	Log("PlaceVegetation2 start loop");
	while (quantity > 0 && attempts++ < 50000) {
		rndx = x + Random(wdt);
		rndy = y + Random(hgt);
		
		if (attempts % 10000 == 0) Log("attempts %d", attempts);
		
		// Okay, we found the correct material, lets try place it somewhere below that material if free
		if (isMaterialSoil(rndx, rndy, materialsoil)) {
			// Search upwards/downwards for free area
			valid = 0; 
			
			var testi = 0;
			while (isMaterialSoil(rndx, rndy, materialsoil) && rndy < y+hgt && rndy > y)  {
				rndy+= yDirection;
				if (testi++ > 50000) {
					Log("odd errror, loop seems infinite. Vars are rndy: %d, y: %d, hgt: %d, ydir: %d",rndy,y,hgt,yDirection);
					return;
				}
				
				if (GetMaterial(rndx, rndy) == freemat) {
					valid = 1;
					break;
				}
				

			}
			// Has to be either in liquid or free of liquid
			valid = valid && ((liquid && GBackLiquid(rndx, rndy)) || (!liquid && !GBackLiquid(rndx, rndy)));
			
			if (valid) {
				// Search upwards/downwards again to see how much free vertical space we have
				var rndy_spacecheck = rndy, testverticalspace = minverticalspace;
				
				var testi = 0;
				while (GetMaterial(rndx, rndy_spacecheck) == freemat && testverticalspace > 0) {
					rndy_spacecheck+= yDirection;
					testverticalspace--;
					
					if (testi++ > 50000) {
						Log("odd errror, loop seems infinite. Vars are rndy: %d, y: %d, hgt: %d, ydir: %d",rndy,y,hgt,yDirection);
						return;
					}
				}
				
				// Ok, enough vertical space
				if (testverticalspace == 0) {
					con = BoundBy(RandomX(cons[0], cons[1]), 1, 100);
					if (havecons && hanging) {
						rndy -= yDirection * (realy_diff * (100-con)) / 100;
					}
					rndy += rndy_diff; // Adjust for root depth etc.
					
					// Actual position retrieved by GetY() is rndy - realy_diff because CreateObject() creates centered to bottom middle and GetX()/GetY() gets object center
					realy = rndy - realy_diff;
					
					if (!FindObject2(Find_ID(objectid), Find_Distance(10, rndx, realy))) {
						obj = CreateObject(objectid, rndx, rndy);
						if (havecons) {
							obj->SetCon(con);
						}
						if (haveautorotates && Random(2)) { // Not always autorotate
							var bot_dy = (GetCon(obj)*GetDefHeight(objectid))/200;
							
							var lefty = obj->GetYBorder(-2, bot_dy, -2*yDirection, 30);
							var righty = obj->GetYBorder(+2, bot_dy, -2*yDirection, 30);
							if (lefty != -1 && righty != -1) {
								obj->SetR2(BoundBy((Angle(-2, lefty, 2, righty)-90)/4, autorotates[0], autorotates[1]), 0, bot_dy - vegetationRootDepth);
							}
						}
						//obj->~PlacedByScript();
						quantity--;
						placed++;
					}
				}
			}
		}
		
	}
	return placed;
}

global func isMaterialSoil(x, y, materialsoil) {
	if (materialsoil != 0) {
		return(GetMaterial(x,y)==materialsoil);
	} else {
		return(GetMatSoil(GetMaterial(x,y)));
	}
}
