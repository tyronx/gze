/*-- Fish --*/

#strict 2
#include AIBA // A.I. Base

local fullness, threatObj;

local stuckTimer;
local stuckX, stuckY;

local swimmingTimer;
local schoolTarget;

func GetAnimalPlacementMaterial() { return Material("Water"); }
func IsFish() { return 1; }


func Initialize() { 
	_inherited();
	
	if(GetAction() == "Idle") {
		return InitFish();
	}
}

/*func FxActivityTimer() {
	_inherited();
	Message("%s", this, activity);
}*/

func ActivityInit() { 
	fullness = RandomX(0, 50);
	AddActivities(["Fleeing", "Sleep", "Feed", "Swim"]);
}

func ShouldExecuteFleeing() {
	// Don't flee if cant flee
	if (Contained() || !InLiquid())  {
		return 0;
	}
	
	// Flee from danger
	for (var threat in FindObjects(
		Find_Distance(200),
		Find_Category(C4D_Living | C4D_Object | C4D_Vehicle | C4D_Structure),
		Find_Not(Find_Category(C4D_Background | C4D_Parallax)), 
		Find_Or(Find_Func("IsClonk"), Find_Not(Find_OCF(OCF_Alive))), // Only avoid clonks or inanimate objects
		Find_NoContainer(),  // Not conatined
		Find_OCF(OCF_InLiquid), // In water
		Sort_Distance()
	)) {
		// Don't escape small objects up to 3x3 (e.g. air bubble)
		if (GetDefWidth(GetID(threat)) + GetDefHeight(GetID(threat)) < 10) continue;
		// Bedrohung bewegt sich nicht?
		if (Abs(GetXDir(threat)) + Abs(GetYDir(threat)) < 4 && GetAction(threat) != "Swim") continue;
		// Kontakt zum Boden?
		if (GetContact(threat, -1, 8)) continue;
		// Keine unsichtbaren Objekte
		if (GetVisibility(threat)) continue;
		
		threatObj = threat;
		fleeThreat(threat);
		
		SetPhysical("Swim", 150000, 2);
		
		return 1;
	}

	return 0;
}

func fleeThreat(threat) {
	var attempts = 0;
	var fleex, fleey, bestdistance;
	
	var xdirection = GetX() - GetX(threat);
	var ydirection = GetY() - GetY(threat);
	
	if (ydirection == 0) {
		ydirection = Random(2)*2 - 1;
	} else {
		ydirection = ydirection / Abs(ydirection);
	} 
	if (xdirection == 0) {
		xdirection = Random(2)*2 - 1;
	} else {
		xdirection = xdirection / Abs(xdirection);
	}
	
	while (attempts++ < 4) {
		var x = RandomX(20, 200) * xdirection;
		var y = RandomX(20, 200) * ydirection;
		var distance = Distance(GetX()+x, GetY()+y, GetX(threat), GetY(threat));
		
		if (GBackLiquid(x,y) && PathFree(GetX(), GetY(), x, y) && distance > bestdistance) {
			fleex = x;
			fleey = y;
			bestdistance = distance;
		}
	}
	
	if (!bestdistance) {
		fleex = RandomX(-100, 100);
		fleey = RandomX(-100, 100);
	}
	
	SetCommand(this(), "MoveTo", 0, GetX() + fleex, GetY() + fleey, 0, false);
}

func ContinueExecuteFleeing() {
	var threat = GetCommand(this, 1);
	
	if (!threat || ObjectDistance(threat) > 200 || Contained() || gotStuck()) {
		SetPhysical("Swim", 65000, 2);
		return 0;
	}
	
	fleeThreat(threat);
	return 1;
}



func ShouldExecuteSleep() {
	return IsNight();
}

func ContinueExecuteSleep() {
	SetComDir(COMD_Down);
	SetCommand(this, "None");
	return IsNight();
}


func ShouldExecuteSchooling() {
	if (buildSchool()) {
		SetCommand(this, "None");
		return 1;
	}
	return 0;
}

func ContinueExecuteSchooling() {
	//Message("schooling", this);
	if (GetCommand() != "MoveTo" || ObjectDistance(schoolTarget) > 20) {
		if (buildSchool()) {
			return 1;
		} else {
			SetPhysical("Swim", 65000, 2);
			return 0;
		}
	}
	if (gotStuck()) {
		SetPhysical("Swim", 65000, 2);
		return 0;
	}
	return 1;
	
}


func buildSchool() {
	for (var pFish in FindObjects(Find_Exclude(this), Find_ID(FISH), Find_Distance(300), Sort_Distance())) {
		if (Local(0) == pFish->Local(0) && LocalN("activity", pFish) != "Schooling") {
			if (ObjectDistance(pFish) > 15) {
				SetCommand(
					this,
					"MoveTo", 
					0,
					GetX(pFish) + RandomX(10,20) * Random(2)*2-1, 
					GetY(pFish) + RandomX(10,20) * Random(2)*2-1,
					0,
					true
				);
				schoolTarget = pFish;
				SetPhysical("Swim", 75000, 2);
				return 1;
			} else {
				return 0;
			}
		}
	}
	return 0;
}


func ShouldExecuteFeed() {
	if (Random(2) && fullness > 0) fullness--;
	if (fullness<= 0) {
		var bestfood;
		for (var food in FindObjects(Sort_Distance(), Find_Distance(200), Find_Func("FishFoodQuality"), Find_OCF(OCF_InLiquid))) {
			if (!bestfood || food->FishFoodQuality() > bestfood->FishFoodQuality()) {
				if (Random(3)) {
					bestfood = food;
				}
			}
		}
		if (bestfood && Random(100) < bestfood->FishFoodQuality()) {
			SetCommand(this(), "Follow", bestfood);
			return 1;
		}
	}
	return 0;
}

func ContinueExecuteFeed() {
	if (GetCommand(this, 1) && ObjectDistance(GetCommand(this, 1)) < 10) {
		fullness+= RandomX(5, 15);
	} 
	
	if (!GetCommand() || gotStuck() || !Random(20)) {
		SetCommand(this(), "None");
		return 0;
	}
	
	
	return fullness < 50 || !GetCommand();
}


func ShouldExecuteSwim() {
	SetCommand(this, "None");
	swimmingTimer = 0;
	return 1;
}

func ContinueExecuteSwim() {
	if (!GetCommand()) {
		// Don't Scratch the Lake surface
		if (GetMaterial(0,-2) == -1) {
			SetCommand(this, "MoveTo", 0, RandomX(0, LandscapeWidth()), GetY() + RandomX(3, LandscapeHeight()/10), 0, 1);
		} else {
			SetCommand(this, "MoveTo", 0, RandomX(0, LandscapeWidth()), GetY() + RandomX(-LandscapeHeight()/10, LandscapeHeight()/10), 0, 1);
		}
	}
	
	if ((GBackSolid(10, -5)  || GBackSolid(-10, -5) || GBackSolid(10, 5)  || GBackSolid(-10, 5)) && swimmingTimer > 1) return 0;

	if (GetMaterial(0,-2) == -1 && GetCommand(this, 3) < GetY()) {
		return 0;
	}
	
	swimmingTimer++;
	
	return !gotStuck();
}


func gotStuck() {
	/* Stuck in one spot prevention */
	if (stuckX == GetX() && stuckY == GetY()) {
		if (stuckTimer++ > 1) {
			stuckTimer = 0;
			return 1;
		}
	}
	stuckX = GetX();
	stuckY = GetY();
	return 0;
}



public func Entrance(container)  {
	// Damit der Fisch nicht aus U-Booten flieht und so.
	SetCommand(this(), "None");
}



public func Eat(object pByObject) {
	pByObject->~Feed(GetCon()/2);
	RemoveObject();
	return 1;
}

public func Cook() {
	ChangeDef(CFSH);
	SetAlive(0,this);
	return 1;
}

protected func Death() {
	SetDir(0);
	ChangeDef(DFSH);
	SetAction("Dead");
	return 1;
}

public func InitFish() {
	var pEnv;
	if (pEnv=FindObject(CLFS)) {
		pEnv->CLFS::Colorize(this);
	} else {
		SetColorDw(RGB(255,255,255));
	}
	SetAction("Swim");
}