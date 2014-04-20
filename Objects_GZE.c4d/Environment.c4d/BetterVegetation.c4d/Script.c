/*-- Bessere Vegetation --*/

#strict 2

local vegetationlevel;


protected func Initialize() {
	SetPosition(0,0);
	
	Log("Replacing Vegetation");
	
	SetVegetationLevel();
	RemoveEnginePlacedVegetation();
	RepopulateVegetation();
	
	RemoveObject();
}


public func VegetationMultiplier() {
	return ((vegetationlevel * Sqrt(LandscapeWidth() * LandscapeHeight())) / 2000);
}

private func SetVegetationLevel() {
	vegetationlevel = BoundBy(
		GetScenarioVal ("VegetationLevel", "Landscape", 0) + RandomX(-GetScenarioVal ("VegetationLevel", "Landscape", 1), GetScenarioVal ("VegetationLevel", "Landscape", 1)), 
		GetScenarioVal ("VegetationLevel", "Landscape", 2), 
		GetScenarioVal ("VegetationLevel", "Landscape", 3)
	);
}



/* Removes the joke of a vegetation from the whole landscape */
public func RemoveEnginePlacedVegetation() {
	var idx=0;
	while (GetScenarioVal ("Vegetation", "Landscape", idx) != 0) {
		//Log("remove %v", GetScenarioVal ("Vegetation", "Landscape", idx));
		RemoveObjects(Find_ID(GetScenarioVal ("Vegetation", "Landscape", idx)));
		idx+=2;
	}
}

/* Places the whole Vegetation again with the help of global fund PlaceVegetation2 */
public func RepopulateVegetation() {
	var idx = 0;
	var vegeid, relativequantity;
	var totalquantity = 0;
	var quantitytypes = 0;
	
	while (GetScenarioVal ("Vegetation", "Landscape", idx) != 0) {
		idx++;
		totalquantity += GetScenarioVal ("Vegetation", "Landscape", idx);
		idx++;
		quantitytypes++;
	}
	
	idx=0;
	while (GetScenarioVal ("Vegetation", "Landscape", idx) != 0) {
		vegeid = GetScenarioVal ("Vegetation", "Landscape", idx++);
		relativequantity = GetScenarioVal ("Vegetation", "Landscape", idx++);
		
		Log("placing %v", vegeid);
		
		var vegebalance = DefinitionCall(vegeid, "GetVegetationPlacementBalance");
		if (!vegebalance) vegebalance = 100;
		
		// (  Plant-Specific Multiplicater  *  types of plants  *  Global Multiplicator) / total sum of all selected vegetation
		var quantity =  (vegebalance * quantitytypes * VegetationMultiplier() * relativequantity) / (100*totalquantity);  // 100 from vegebalance being in percent
		
		//Log ("place %d of %v   (%d * %d * %d * %d) / (100*%d)", quantity, vegeid, vegebalance, quantitytypes, VegetationMultiplier(), relativequantity, totalquantity);
		
		var y = (LandscapeHeight()*DefinitionCall(vegeid, "GetVegetationLineTop"))/100;
		
		var hgt = (LandscapeHeight()*DefinitionCall(vegeid, "GetVegetationLineBottom"))/100 - y;
		if (hgt == 0 || hgt < 0) hgt = LandscapeHeight()-y;
		
		var materialsoil = DefinitionCall(vegeid, "GetVegetationSoil"); 
		if (materialsoil) materialsoil=Material(materialsoil);
		
		var underground = DefinitionCall(vegeid, "IsUndergroundVegetation");
		var submergedliquid = DefinitionCall(vegeid, "IsSubmergedVegetation");
		if (submergedliquid) submergedliquid = Material(submergedliquid);
		var hanging  = DefinitionCall(vegeid, "IsHangingVegetation");
		var rotates = DefinitionCall(vegeid, "GetVegetationRotates");
		var cons = DefinitionCall(vegeid, "GetVegetationCons");
		
		PlaceVegetation2 (vegeid, quantity, [0, y, LandscapeWidth(), hgt], materialsoil, underground, submergedliquid, hanging, cons, rotates);
	}
}