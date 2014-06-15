/*-- Fish --*/

#strict 2
#include AIBA // A.I. Base

local fullness, threatObj;

local stuckTimer;
local stuckX, stuckY;


func GetAnimalPlacementMaterial() { return Material("Water"); }


func Initialize() { 
	_inherited();
	
	if(GetAction() == "Idle") {
		return InitFish();
	}
}


func ActivityInit() { 
	fullness = RandomX(0, 50);
	AddActivities(["Fleeing", "Sleep", "Feed", "Swim"]);
}

func ShouldExecuteFleeing() {
	// Flee from danger
	if (!Contained() && InLiquid())  {
		return 0;
	}
	
	for (var threat in FindObjects(
		Find_Distance(100),
		Find_Category(C4D_Living | C4D_Object | C4D_Vehicle | C4D_Structure),
		Find_Not(Find_Category(C4D_Background | C4D_Parallax)), 
		Find_Not(Find_ID(FXU1)), // Air bubble
		Find_Or(Find_Func("IsClonk"), Find_Not(Find_OCF(OCF_Alive))), // Only avoid clonks or inanimate objects
		Find_NoContainer(),  // Not conatined
		Find_OCF(OCF_InLiquid), // In water
		Sort_Distance()
	)) {
		// Bedrohung bewegt sich nicht?
		if (Inside(GetXDir(threat), -2, +2) && Inside(GetYDir(threat), -3, +3)) continue;
		// Kontakt zum Boden?
		if (GetContact(threat, -1, 8)) continue;
		// Keine unsichtbaren Objekte
		if (GetVisibility(threat)) continue;
		
		threatObj = threat;
		fleeThreat(threat);
		return 1;
	}

	return 0;
}

func fleeThreat(threat) {
	var attempts = 0;
	var fleex, fleey, bestdistance;
	while (attempts++ < 8) {
		var x = RandomX(20, 80) * (Random(2)*2 - 1);
		var y = RandomX(20, 80) * (Random(2)*2 - 1);
		var distance = Distance(GetX()+x, GetY()+y, GetX(threat), GetY(threat));
		
		if (GBackLiquid(x,y) && distance > bestdistance) {
			fleex = x;
			fleey = y;
		}
	}
	
	SetCommand(this(), "MoveTo", 0, GetX() + fleex, GetY() + fleey, 0, true);
}

func ContinueExecuteFleeing() {
	// Wenn keine Bedrohung mehr oder nicht im Wasser aufhören zu fliehen
	if (!InLiquid() || Contained() || gotStuck()) {
		SetCommand(this(), "None");
		return 0;
	}
	
	return GetCommand() == "MoveTo";
}



func ShouldExecuteSleep() {
	return IsNight();
}

func ContinueExecuteSleep() {
	SetComDir(COMD_Down);
	SetCommand(this, "None");
	return IsNight();
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
		}
		return 1;
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
	
	
	return fullness < 50;
}


func ShouldExecuteSwim() {
	SetCommand(this, "None");
	return 1;
}

func ContinueExecuteSwim() {
	if (!GetCommand() || Abs(GetX() - GetCommand(this, 2)) < 10 || Abs(GetY() - GetCommand(this, 3)) < 10) {
		var x,y, attempts = 13;
		while (attempts-- > 0) {
			if (GBackLiquid(x += RandomX(-80, 80), y += RandomX(-80, 80)) && GetMaterial(x, y) == GetAnimalPlacementMaterial()) {
				SetCommand(this, "MoveTo", 0, GetX() + x, GetY() + y, 0,  1);
				break;
			}
		}
	}
	return 1;
}


func gotStuck() {
	/* Stuck in one spot prevention */
	if (stuckX == GetX() && stuckY == GetY()) {
		if (stuckTimer++ > 2) {
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
	pByObject->~Feed(50);
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