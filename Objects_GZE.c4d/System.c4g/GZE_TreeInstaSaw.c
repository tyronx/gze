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
	
	var treematerial = GetComponent(0, 0);
	var quantity = Round((GetComponent(treematerial) * GetCon())/20);
	
	var i = 0, pObj, attempts = 0, x, y;
	//Log("%d %d %d", quantity, hgt, pos);
	while(i < quantity) {
		i++;
		
		x = -Sin(GetR(), i * (wdt/quantity) - wdt/2)/10;
		y = Cos(GetR(), (i * (hgt/quantity) - hgt/2))/10;
		pObj = CreateObject(treematerial, x, y);
		attempts = 0;
		while (Stuck(pObj) && attempts++ < 100) {
			pObj->SetPosition(GetX()+x, GetY()+y+attempts);
			if (!Stuck(pObj)) break;
			pObj->SetPosition(GetX()+x, GetY()+y-attempts);
			if (!Stuck(pObj)) break;
		}
		if (attempts >= 100) {
			pObj->SetPosition(GetX()+x, GetY()+y);
		}
	}
	//Sound("TreeDown*");
	RemoveObject();
}