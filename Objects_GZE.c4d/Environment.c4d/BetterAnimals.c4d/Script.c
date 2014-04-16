/*-- Better Animal placement --*/

#strict 2

local animalplacementlevel;


protected func Initialize() {
	SetPosition(0,0);
	
	SetEarthObjectsLevel();
	RemoveEnginePlacedAnimals();
	PlaceAnimals();
}


public func AnimalsMultiplier() {
	return ((animalplacementlevel * Sqrt(LandscapeWidth() * LandscapeHeight())) / 2000);
}

private func SetEarthObjectsLevel() {
	animalplacementlevel = 8;
}


public func RemoveEnginePlacedAnimals() {
	var idx=0, objectid;
	while (GetScenarioVal ("Animal", "Animals", idx) != 0) {
		RemoveObjects(Find_OCF(OCF_NotContained), Find_ID(GetScenarioVal("Animal", "Animals", idx)));
		idx+=2;
	}
	
}

/* Places the whole Vegetation again with the help of global fund PlaceVegetation2 */
public func PlaceAnimals() {
	var idx = 0;
	var objid, relativequantity;
	var totalquantity = 0;
	var quantitytypes = 0;
	
	while (GetScenarioVal ("Animal", "Animals", idx) != 0) {
		idx++;
		totalquantity += GetScenarioVal ("Animal", "Animals", idx);
		idx++;
		quantitytypes++;
	}
	
	idx=0;
	while (GetScenarioVal ("Animal", "Animals", idx) != 0) {
		objid = GetScenarioVal ("Animal", "Animals", idx++);
		relativequantity = GetScenarioVal ("Animal", "Animals", idx++);
		
		var objbalance = DefinitionCall(objid, "GetAnimalPlacementBalance");
		if (!objbalance) objbalance = 100;
		
		// (  Plant-Specific Multiplicater  *  types of plants  *  Global Multiplicator) / total sum of all selected obj
		var quantity =  (objbalance * quantitytypes * AnimalsMultiplier() * relativequantity) / (100*totalquantity); 
		
		
		var placement = GetDefPlacement(objid);
		// Script defined placement overrides Defcore value
		if (DefinitionCall(objid, "GetAnimalPlacement")) placement = DefinitionCall(objid, "GetAnimalPlacement");
		//if (objid == WIPF) Log("%d %d", placement, DefinitionCall(objid, "GetAnimalPlacement"));
		var y = (LandscapeHeight()*DefinitionCall(objid, "GetAnimalPlacementLineTop"))/100;
		
		var hgt = (LandscapeHeight()*DefinitionCall(objid, "GetAnimalPlacementLineBottom"))/100 - y;
		if (hgt == 0 || hgt < 0) hgt = LandscapeHeight()-y;
		
		var placementmat = DefinitionCall(objid, "GetAnimalPlacementMaterial"); 
		if (!placementmat) placementmat = -1;
		
		PlaceAnimals2(objid, quantity, [0, y, LandscapeWidth(), hgt], placement, placementmat);
	}
}



