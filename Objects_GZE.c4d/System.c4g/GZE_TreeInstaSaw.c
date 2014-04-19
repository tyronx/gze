/* Sägewerke sind obsolet: Heutzutage werden Bäume Vor-Ort zerkleinert! */
#strict 2
#appendto TREE

local chops;

protected func Damage() {
	for(var pClonk in FindObjects (Find_OCF(OCF_CrewMember))) {
		if (pClonk->GetCommand(0, 1) == this()) {
			chops++;
		}
	}
	if (chops > 6 + Random(5)) {
		ChopDown();
	}

	return (0);
}

public func ChopDown() {
	ScheduleCall(this(), "TurnToWood", 60);
	return(_inherited());
}

public func TurnToWood() {
	var wdt = GetDefCoreVal("Width", "DefCore", GetID()) * GetCon()/10;
	var hgt = GetDefCoreVal("Height", "DefCore", GetID()) * GetCon()/10;
	var quantity = Round((GetComponent(WOOD) * GetCon())/20);
	
	var i = 0;
	//Log("%d %d %d", quantity, hgt, pos);
	while(i < quantity) {
		i++;
		
		CreateObject(WOOD, -Sin(GetR(), i * (wdt/quantity) - wdt/2)/10, Cos(GetR(), (i * (hgt/quantity) - hgt/2))/10);
	}
	//Sound("TreeDown*");
	RemoveObject();
}