/*-- Schmetterling --*/

#strict 2

/*-- Engine-Calls --*/

func GetActivities() { return ["Sleep", "Feed", "Glide", "Fly"]; }
local activity;

local flowerObj;
local feedCooldown;
local sleepCooldown;
local stuckTimer;
local stuckX, stuckY;


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
	
	for (var objplant in FindObjects(Find_Distance(30), Find_Category(C4D_SelectVegetation()))) {
		if (objplant->~GetVegetationIsFlower() && objplant != flowerObj) {
			SetCommand (this(), "Follow", objplant);
			flowerObj = objplant;
			return 1;
		}
	}
	return 0;
}

func ContinueExecuteFeed() {

	if (!flowerObj) return 0;
	
	if (GetAction() == "Sit") {
		if (!Random(5)) {
			SetAction("Feeding");
		}
		feedCooldown++;
		
		if (feedCooldown > 4 && !Random(4)) {
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
	
	if (!Random(100)) {
		return 0;
	}
	
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
			for (var objplant in FindObjects(Find_Distance(300), Sort_Distance(), Find_Category(C4D_SelectVegetation()))) {
				if (objplant->~GetVegetationIsFlower() && Random(2) && objplant != flowerObj) {
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


// Initialisierung
protected func Initialize() {
	if (!Random(3)) {
		SetGraphics("Charaxes");
	}
	SetAction("Fly");
	Activity();
	return;
}

// TimerCall
protected func Activity() {
	if (activity) {
		if (!ObjectCall(this(), Format("ContinueExecute%s", activity))) {
			activity = 0;
		}
	}
	
	for (strActivity in GetActivities()) {
		//Activities are sorted by priority. So if a higher priority activity is running, do not check on others
		if (activity == strActivity) break;
		
		if (ObjectCall(this(), Format("ShouldExecute%s", strActivity))) {
			activity = strActivity;
			break;
		}
	}
	return;
}
  
protected func Death() {
	FadeOut();
}




func ContactLeft() {
	Contact();
}
func ContactBottom() {
	Contact();
}
func ContactTop() {
	Contact();
}
func ContactRight() {
	Contact();
}
func Contact() {
	if (activity == "Glide") {
		activity = "Fly";
		ShouldExecuteFly();
	}
}

