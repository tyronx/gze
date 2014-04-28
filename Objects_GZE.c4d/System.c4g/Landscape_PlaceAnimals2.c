#strict 2

/* 
 * Better Animal Placement. Besides many new parameters it also does the following:
 * - Leaves at least 10 pixels distance between each placed animal
 * - Two new placement types: 3 for underground, 4 for burrow
 * Parameters:
 *
 * objectid:  Object to be placed
 * quantity: Quantity to be planted
 * rect: Target rect as array [x,y,wdt, hgt], Offset in local calls
 * placement: 0 for Surface, 1 for liquid, 2 for air, 3 for underground, 4 for burrow :D
 * placementmat: 
 * - For placement = 0 or 3: the Material it should be place on top of; For placement=1: the liquid it should be placed in (use -1 for any solid or liquid respectively)   
 [* - Can be single material or array] TODO
 * cons: array of min max growth rates of the animal. If not an array, is passed the growth rate will be always 100%
 *
 * Returns:
 * Amount of actually placed objects (algo gives up after 40.000 iterations) 
 *
 * Examples:
 *
 * PlaceAnimals2(BIRD, 50, [0,0, LandscapeWidth(), 500], 2);
 *
 */
global func PlaceAnimals2(id objectid, int quantity, array rect, int placement, int placementmat, array cons) {
	var x = rect[0], y = rect[1], wdt = rect[2], hgt = rect[3];
	var havecons = GetType(cons) == C4V_Array;
	if (!havecons) {
		cons = [0,0];
	}
	
	// Failsafe
	var attempts = 0;
	var placed = 0;
	
	// At least 120% of the objects height must be free vertically
	var minverticalspace = (GetDefHeight(objectid) * 5) / 4;
	// Minimum distance between other animals
	var mindistance = GetDefWidth(objectid);

	// The Material the object will be placed in
	var freemat = -1;  // in air for placement == 0 and placement == 2
	if (placement == 3) freemat = Material("Tunnel");
	if (placement == 1) freemat = placementmat;
	
	// Create a dozen random places and try there
	var rndx, rndy, realy, valid, materialatpos, pAnimal;
	
	// to make the loop a bit more efficient we do this calculation outside the loop - these are adjustments for root depth etc.
	var realy_diff;

	realy_diff = GetDefHeight(objectid) / 2;
	
	
	if (placement == 4) {
		var minhorizontalspace = (RandomX(15, 30) * GetDefWidth(objectid)) / 10;
		
		while (quantity > 0 && attempts++ < 10000) {
			rndx = x + Random(wdt);
			rndy = y + Random(hgt);
		
			// Check a few points if there can be a burrow
			if (isMaterialSoil(rndx, rndy) &&
				isMaterialSoil(rndx-minhorizontalspace/2, rndy-minverticalspace/2) &&
				isMaterialSoil(rndx-minhorizontalspace, rndy) &&
				isMaterialSoil(rndx-minhorizontalspace, rndy-minverticalspace) &&
				isMaterialSoil(rndx, rndy-minverticalspace)
			) {
				
				var digx, digy, radius;
				for (var i = 0; i < minhorizontalspace; i+=2) {
					rndy += RandomX(-3,3);
					
					digx = rndx-minhorizontalspace + i;
					digy = rndy-minverticalspace/2;
					radius = minverticalspace/2 - 3 + RandomX(-3,3);
					
					DigFree(digx, digy, radius);
					for (var pObj in FindObjects(Find_Distance(radius, digx, digy), Find_Category(C4D_SelectInEarth()))) {
						if (!Stuck(pObj)) {
							pObj->RemoveObject();
						}
					}
				}
				pAnimal = CreateObject(objectid, rndx-minhorizontalspace/2, rndy);
				if (havecons) {
					pAnimal->SetCon(BoundBy(RandomX(cons[0], cons[1]), 1, 100));
				}
				placed++;
				quantity--;
				
			}
		}
		
	} else {

		while (quantity > 0 && attempts++ < 40000) {
			rndx = x + Random(wdt);
			rndy = y + Random(hgt);
			materialatpos = GetMaterial(rndx, rndy);
			
			valid = (placement == 0 || placement == 3) && ((placementmat>=0 && materialatpos == placementmat) || (placementmat==-1 && GBackSolid(rndx, rndy)));
			
			// On surface or in caves => search upwards vertically
			while (valid && GetMaterial(rndx, rndy) != freemat) {
				valid = (placementmat>=0 && GetMaterial(rndx, rndy) == placementmat) || (placementmat==-1 && GBackSolid(rndx, rndy));
				rndy--;
			}
		
			// We got air for air placement
			valid = valid || placement == 2 && materialatpos == -1;
			
			// We got the right liquid for liquid placement
			valid = valid || (placement == 1 && ((placementmat>=0 && materialatpos == placementmat) || (placementmat==-1 && GBackLiquid(rndx, rndy))));
			
			//Log("%d %d", placement, materialatpos);
			if (valid) {
				// Search upwards/downwards again to see how much free vertical space we have
				var rndy_spacecheck = rndy, testverticalspace = minverticalspace;
				
				while (valid && testverticalspace > 0) {
					valid = ((placement != 1 || placementmat>=0) && GetMaterial(rndx, rndy_spacecheck) == freemat) || (placement == 1 && placementmat==-1 && GBackLiquid(rndx, rndy_spacecheck));
					rndy_spacecheck--;
					testverticalspace--;
				}
			}
			
			// Ok, enough vertical space
			if (valid) {
				// Actual position retrieved by GetY() is rndy - realy_diff because CreateObject() creates centered to bottom middle and GetX()/GetY() gets object center
				realy = rndy - realy_diff;
				
				if (!FindObject2(Find_ID(objectid), Find_Distance(mindistance, rndx, realy))) {
					pAnimal = CreateObject(objectid, rndx, rndy);
					if (havecons) {
						pAnimal->SetCon(BoundBy(RandomX(cons[0], cons[1]), 1, 100));
					}
					//Log("%v %d %d", obj, rndx, rndy);
					//obj->~PlacedByScript();
					quantity--;
					placed++;
				}
			}
			
		}
	}
	return placed;
}
