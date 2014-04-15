/*-- Better Earth Objects --*/

#strict 2

local earthobjectslevel;


protected func Initialize() {
	SetPosition(0,0);
	
	SetEarthObjectsLevel();
	RemoveEnginePlacedEarthObj();
	PlaceEarthObjects();
}


public func InEarthObjectsMultiplier() {
	return ((earthobjectslevel* Sqrt(LandscapeWidth() * LandscapeHeight())) / 500);
}

private func SetEarthObjectsLevel() {
	earthobjectslevel = BoundBy(
		GetScenarioVal ("InEarthLevel", "Landscape", 0) + RandomX(-GetScenarioVal ("InEarthLevel", "Landscape", 1), GetScenarioVal ("InEarthLevel", "Landscape", 1)), 
		GetScenarioVal ("InEarthLevel", "Landscape", 2), 
		GetScenarioVal ("InEarthLevel", "Landscape", 3)
	);
}



public func RemoveEnginePlacedEarthObj() {
	var idx=0, objectid;
	while (GetScenarioVal ("InEarth", "Landscape", idx) != 0) {
		for (var obj in FindObjects(Find_ID(objectid = GetScenarioVal ("InEarth", "Landscape", idx)))) {
			if (GetMaterial(obj->GetX(), obj->GetY() + GetDefHeight(objectid)/2) == Material("Earth") || !Stuck(obj)) {
				RemoveObject(obj);
			}
		}
		idx+=2;
	}
}

/* Places the whole Vegetation again with the help of global fund PlaceVegetation2 */
public func PlaceEarthObjects() {
	var idx = 0;
	var objid, relativequantity;
	var totalquantity = 0;
	var quantitytypes = 0;
	
	while (GetScenarioVal ("InEarth", "Landscape", idx) != 0) {
		idx++;
		totalquantity += GetScenarioVal ("InEarth", "Landscape", idx);
		idx++;
		quantitytypes++;
	}
	
	idx=0;
	while (GetScenarioVal ("InEarth", "Landscape", idx) != 0) {
		objid = GetScenarioVal ("InEarth", "Landscape", idx++);
		relativequantity = GetScenarioVal ("InEarth", "Landscape", idx++);
		
		var objbalance = DefinitionCall(objid, "GetEarthObjPlacementBalance");
		if (!objbalance) objbalance = 100;
		
		// (  Plant-Specific Multiplicater  *  types of plants  *  Global Multiplicator) / total sum of all selected obj
		var quantity =  (objbalance * quantitytypes * InEarthObjectsMultiplier() * relativequantity) / (100*totalquantity); 
		
		
		
		var y = (LandscapeHeight()*DefinitionCall(objid, "GetEarthObjLineTop"))/100;
		
		var hgt = (LandscapeHeight()*DefinitionCall(objid, "GetEarthObjLineBottom"))/100 - y;
		if (hgt == 0 || hgt < 0) hgt = LandscapeHeight()-y;
		
		var materials = DefinitionCall(objid, "GetEarthObjMaterials"); 
		
		PlaceObjects2(objid, quantity, [0, y, LandscapeWidth(), hgt], materials);
	}
}



