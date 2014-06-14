/*-- Butterfly --*/

#strict 2

#include AIBA // AI Base

local flowerObj;
local feedCooldown;
local sleepCooldown;
local stuckTimer;
local stuckX, stuckY;


func Initialize() {
	if (!Random(3)) {
		SetGraphics("Charaxes");
	}
	return _inherited();
}

func ActivityInit() {
	AddActivities(["Sleep", "Feed", "Glide", "Fly"]);
}

func ShouldExecuteSleep() {
	if (sleepCooldown > 0 && sleepCooldown-- > 0) return 0;
	
	if (IsNight()) {
		SetCommand(this, "None");
		return 1;
	}
	
	return 0;
}

func ContinueExecuteSleep() {
	if (GetCommand() != "Follow" && GetAction() != "Sit") {
		for (var objplant in FindObjects(Sort_Distance(), Find_Category(C4D_SelectVegetation()))) {
			if (Random(2)) {
				SetCommand (this(), "Follow", objplant);
				return 1;
			}
		}
	}
	if (GetCommand() == "Follow") {
		if (!GetCommand(this, 1)) {
			SetCommand(this, "None");
			return 1;
		}
		var objid = GetID(GetCommand(this, 1));
		var xdist = Abs(GetX() - GetX(GetCommand(this, 1)));
		var ydist = Abs(GetY() - GetY(GetCommand(this, 1)));
		
		if (xdist < GetDefWidth(objid) / 2 && ydist < GetDefHeight(objid) - 4) {
			SetCommand(this, "None");
			SetSpeed(0,0);
			SetComDir(COMD_None);
			SetAction("Sit");
		}
	}
	if (FindObject2(Find_Distance(50), Find_OCF(OCF_CrewMember), Find_NoContainer())) {
		SetAction("Fly");
		sleepCooldown = 5;
		return 0;
	}
	
	return IsNight();
}

func ShouldExecuteFeed() {
	if (feedCooldown > 0 && feedCooldown-- > 0) return 0;
	
	var objplant = FindObject2(
		Find_Distance(30), 
		Find_Func("GetVegetationIsFlower"), 
		Find_Category(C4D_SelectVegetation()),
		Find_Exclude(flowerObj)
	);
	if (objplant) {
		SetCommand(this(), "Follow", objplant);
		flowerObj = objplant;
		return 1;
	}
	return 0;
}

func ContinueExecuteFeed() {
	if (!flowerObj) return 0;
	
	if (GetAction() == "Sit") {
		if (!Random(4)) {
			SetAction("Feeding");
		}
		feedCooldown++;
		
		if (feedCooldown > 5 && !Random(5)) {
			return 0;
		}
	}
	
	if (ObjectDistance(flowerObj) < 12 && GetAction() != "Sit" && GetAction() != "Feeding" && GetAction() != "Feeding2") {
		SetSpeed(0, 0);
		SetComDir(COMD_None);
		SetCommand(this(), "None");
		SetAction("Sit");
	}
	
	if (FindObject2(Find_Distance(50), Find_OCF(OCF_CrewMember), Find_NoContainer())) {
		feedCooldown = 10;
		return 0;
	}
	
	/* Stuck in one spot prevention */
	if (stuckX == GetX() && stuckY == GetY()) {
		if (stuckTimer++ > 3) {
			stuckTimer = 0;
			SetCommand(this(), "None");
			return 0;
		}
	}
	stuckX = GetX();
	stuckY = GetY();

	
	return 1;
}

func ShouldExecuteGlide() {
	if (activity == "Fly" && GetActTime() > 30 && (GetPhase() == 2 || GetPhase() == 8) && !Random(3) && Abs(GetXDir()) > 8) {
		SetAction("Glide");
		SetCommand(this(), "None");
		SetComDir(COMD_None);
		return 1;
	}
	return 0;
}

func ContinueExecuteGlide() {
	if (GetYDir() < 10) {
		SetYDir(GetYDir() + 2);
	}
	if (Random(GetActTime()/15)) {
		return 0;
	}
	return 1;
}


func ShouldExecuteFly() {
	SetAction("Fly");
	return 1;
}

func ContinueExecuteFly() {
	if (!GetCommand() || (GetCommand() != "MoveTo" && GetCommand() != "Follow") || (GetCommand() == "Follow" && GetCommand(this, 1) == flowerObj)) {
		SetCommand(this(), "None");
		if (Random(2)) {
			for (var objplant in FindObjects(
				Find_Distance(300), 
				Sort_Distance(), 
				Find_Category(C4D_SelectVegetation()), 
				Find_Func("GetVegetationIsFlower"),
				Find_Exclude(flowerObj)
			)) {
				if (Random(2)) {
					SetCommand (this(), "MoveTo", objplant);
				}
			}
		}
		if (!GetCommand()) {
			var iX, iY, tries = 30;
			while (!GBackSky(iX += RandomX(-200, 200), iY += RandomX(-200, 200)) && tries-- > 0) {}
			
			SetCommand(this, "MoveTo", 0, GetX() + iX, GetY() + iY, 0, 1);
		}
	}
	
	/* Stuck in one spot prevention */
	if (stuckX == GetX() && stuckY == GetY()) {
		if (stuckTimer++ > 3) {
			stuckTimer = 0;
			SetCommand(this(), "None");
			return 0;
		}
	}
	stuckX = GetX();
	stuckY = GetY();
	
	return 1;
}


  
protected func Death() {
	FadeOut();
}

func Contact(direction) {
	if (activity == "Glide") {
		activity = "Fly";
		ShouldExecuteFly();
	}
}