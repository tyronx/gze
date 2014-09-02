/*-- Ballon --*/

local target_y, nopilottimer;

#strict 2
#include VETR

// Needs to be relative Coordinates x,y,wdt,hgt  
func VehicleGrabArea() { return [-20, -5, 40, 30]; }
func VehicleHoldingArea() { return [-13, 0, 26, 25]; }

// Example func to determine when to relay clonk controls to the held vehicle
func CanRelayControl(controltype, pClonk) { 
	if (controltype == "ControlUp" || controltype == "ControlDown") {
		return (GetAction() == "WaterLand" || GetAction() == "FloatIdle") && GetYDir() <= 2 && GetXDir()==0;
	}
	return 1; 
}

/* Initialisierung */

protected func Initialize() {
	target_y = -1;
	SetAction("Float");
	SetComDir(COMD_Down);
	return inherited();
}

/* Steuerung */

public func ControlUp(object controller) {
	[$TxtRise$]
	ClearCommand();
	if (GetAction() == "WaterLand") {
		SetAction("WaterLaunch");
	}

	if (!GetPlrJumpAndRunControl(controller->GetController())) {
		var comdir = GetComDir();
		if (GBackSolid(0, GetObjHeight()/2+2)) {
			comdir = COMD_Stop;
		}

		if (comdir == COMD_Down) {
			SetDirection(COMD_Stop);
		} else {
			SetDirection(COMD_Up);
		}
	}
	
	return inherited(controller);
}

public func ControlDownSingle(object controller) {
	[$TxtLower$|Method=Classic]
	ClearCommand();

	if (!GetPlrJumpAndRunControl(controller->GetController())) {
		if (GetComDir() == COMD_Up) {
			SetDirection(COMD_Stop);
		} else {
			SetDirection(COMD_Down);
		}
	}
	return inherited(controller);
}

public func ControlDown(object controller) {
	[$TxtLower$|Method=JumpAndRun]
	return inherited(controller);
}

private func ClearDir() {
	if(GetYDir()) {
		if(GetYDir() > 0) {
			SetYDir(GetYDir() - 1);
		} else { 
			SetYDir(GetYDir() + 1);
		}
	}
}

public func ControlUpdate(object controller, int comdir) {
	SetDirection(comdir);
}

public func SetDirection(int comdir) {
	if (comdir == COMD_Up || comdir == COMD_UpLeft || comdir == COMD_UpRight) {
		SetComDir(COMD_Up);
	} else if (comdir == COMD_Down || comdir == COMD_DownLeft || comdir == COMD_DownRight) {
		SetComDir(COMD_Down);
	} else {
		ScheduleCall(this(), "ClearDir", 1, Abs(GetYDir()));
		SetComDir(COMD_Stop);
	}

	CheckWindY();
}

protected func ControlCommand(string szCommand, object pTarget, int iX, int iY) {
	// Pilot läßt los
	if (szCommand == "UnGrab") {
		ClearCommand();
		return 0;
	}
	// Bewegungskommando vom Piloten
	if (szCommand == "MoveTo") {
		target_y = iY;
		return 1;
	}
}

private func ClearCommand() { 
	target_y = -1; 
}

/* Aktivität */

protected func Wind2Flag() { 
	SetDir(BoundBy(GetWind()/10+4,0,8)); 
}

private func Wind2Float() {
	if (!GetContact(0, -1, 8)) {
		SetXDir( BoundBy(GetWind()/4,-10,+10));
	}
	if (!FindObject(0,0,0,0,0,0,"Push",this())) {
		nopilottimer++;
	} else {
		nopilottimer=0;
	}
	if (nopilottimer > 1) {
		SetComDir(COMD_Down);
	} else if (target_y != -1) {
		SetYDir(BoundBy((target_y - GetY())/4, -10, 10) );
		SetComDir(COMD_None);

		if (Inside(GetY(), target_y - 5, target_y + 5)) {
			target_y = -1;
		}
	}

	CheckWindY();
	Wind2Flag();
}

private func CheckWindY() {
	// Ohne Kommanda ein bisschen rauf- und runter geweht werden
	if(GetComDir() == COMD_Stop && target_y == -1 && !GBackSolid(0, GetObjHeight()/2 + 2)) {
		if (!GetEffect("IntWindYDir", this())) {
			AddEffect("IntWindYDir", this(), 1, 10, this());
		}
	} else {
		if (GetEffect("IntWindYDir", this())) {
			RemoveEffect("IntWindYDir", this());
		}
		// Wenn der Ballon am Boden ist ComDir auf COMD_Down setzen,
		// sonst hebt der Ballon zu schnell wieder ab
		if(GetComDir() == COMD_Stop && target_y == -1) {
			SetComDir(COMD_Down);
		}
	}
}

protected func FxIntWindYDirTimer(object target, int number, int time) {
	SetYDir(BoundBy(GetYDir() + RandomX(-GetYDir()) - 1 + Random(3), Max(GetYDir() - 1, -3), Min(GetYDir() + 1, 3)));
}

/* Trefferkontrolle */

protected func RejectCollect(id idObject, object pObj) {
	// Pfeile vertragen sich nicht mit einem Ballon
	if (DefinitionCall(idObject,"IsArrow")) {
		// Nur, wenn der Pfeil sich auch bewegt
		if(GetXDir(pObj) || GetYDir(pObj)) {
			// bei brennendem Pfeil anzünden
			if(OnFire(pObj)) {
				Incinerate();
			} else {
				// ansonsten abstürzen
				SetAction("DropOff");
			}
		}
		// in jedem Fall im Pfeil Hit auslösen (Sprengpfeil explodiert zB)
		ProtectedCall(pObj, "Hit");
		return 1;
	}
	return 1;
}

protected func Collection(object pObj) {
	var idObj = GetID(pObj);
	// bei brennendem Pfeil anzünden
	if(OnFire(pObj)) {
		Incinerate();
	} else {
		// ansonsten abstürzen
		SetAction("DropOff");
	}
	// in jedem Fall im Pfeil Hit auslösen (Sprengpfeil explodiert zB)
	ProtectedCall(pObj, "Hit");
	return 1;
}

/* Externe Einwirkungen */

public func Poke() {
	Sound("Rip");
	SetAction("DropOff");
}
