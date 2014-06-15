/*-- Animal respawning--*/

#strict

func Initialize() {
	ScheduleCall(this(), "LateInitialize", 1);
}

func LateInitialize() {
	SetPosition(0,0);
	
	var idx=0;
	while (GetScenarioVal ("Animal", "Animals", idx) != 0) {
		var objid = GetScenarioVal ("Animal", "Animals", idx);
		SetComponent(objid, ObjectCount(objid));
		idx+=2;
	}
}

func CheckPopulation() {
	var cid, i=0;
	while (cid = GetComponent(0, i++)) {
		if (ObjectCount(cid) < GetComponent(cid) && !Random(5)) {
			SpawnAnimal(cid);
		}
	}
}


func SpawnAnimal(objid) {
	var placement = GetDefPlacement(objid);
	// Script defined placement overrides Defcore value
	if (DefinitionCall(objid, "GetAnimalPlacement")) placement = DefinitionCall(objid, "GetAnimalPlacement");

	var y = (LandscapeHeight()*DefinitionCall(objid, "GetAnimalPlacementLineTop"))/100;
	
	var hgt = (LandscapeHeight()*DefinitionCall(objid, "GetAnimalPlacementLineBottom"))/100 - y;
	if (hgt == 0 || hgt < 0) hgt = LandscapeHeight()-y;
	
	var placementmat = DefinitionCall(objid, "GetAnimalPlacementMaterial"); 
	if (!placementmat) placementmat = -1;
	
	PlaceAnimals2(objid, 1, [0, y, LandscapeWidth(), hgt], placement, placementmat);
}