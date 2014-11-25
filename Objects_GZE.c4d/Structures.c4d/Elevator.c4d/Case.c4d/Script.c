/*-- Fahrstuhlkorb --*/

#strict 2
#include VETR

// Local 8: Obergrenze
// Local 9: Untergrenze

local AutoMode;       // Dauerbetrieb
local AutoMoveDir;    // Dauerbetrieb Richtung
local RangeTop;       // Obergrenze
local RangeBottom;    // Untergrenze
local timer;          // Timer

local oElev;          // Das Fahrstuhl-Objekt

/* TimerCall */

protected func Timer()  {
	timer = (++timer) % 30;
	// Feststecken -> Schachtbohrung
	if (Stuck() && GetComDir() != COMD_Stop) {
		if (!NoEnergy()) {
			DigFreeRect(GetX() - 12, GetY() - 13, 24, 26);
		}
	}
	if (!timer) Activity();
}

protected func Riding()  {
	var elev = Elevator();
	if (!elev) return Halt();
	// Energieverbrauch
	if (ObjectCount(ENRG))
		if (EnergyCheck(25000, elev))
			DoEnergy(-25000, elev, true);
		else
			Halt();
}
	
protected func Activity() {
	UpdateTransferZone();
	// Legacy
	if (Local(8) && !RangeTop) ImportSettings();
	// Fahrstuhl zerstört
	// if (!Elevator() || OnFire(Elevator())) { SetAction("Idle"); Incinerate(); return(); }
	// Zu wartenden Clonks fahren
	if (AutoMode == 0 && !Passenger() && !GetCommand())
		MoveToWaitingClonk();
	// Sicherheitscheck: Bewegung ohne Kommando nur bei Schachtbohrung
	if (GetComDir())
		if (!GetCommand() && GetAction() != "Drill")
			Halt();
	// Fertig
	return(1);
}

private func MoveToWaitingClonk() {
	// Wartender Clonk da? 
	var oClnk;
	if(!(oClnk = FindWaitingClonk()) ) return(0);

	// Bewegung nötig?
	if(Abs(GetY(oClnk) - GetY()) < 4 
		 && GetProcedure(oClnk) != "SWIM") return 0;

	// Zielposition bestimmen
	var iTargetY = GetY(oClnk);
	if(GetProcedure(oClnk) == "SWIM")
		iTargetY -= 2;
	if(GetY() > iTargetY)
		iTargetY -= 3;
	
	// Dorthin fahren
	SetMoveTo(iTargetY);
	return(1);              
}

private func FindWaitingClonk() {
	var clnk, best, proc;
	while(clnk = FindObject(0, -30, RangeTop - GetY(), 60, 6000, OCF_CrewMember, 0, 0, NoContainer(), clnk) ) {
		proc = GetProcedure(clnk);
		if(GetComDir(clnk) == COMD_Stop || (proc == "SWIM" && Inside(GetXDir(clnk), -5, 5)) ) {
			if(proc == "WALK" || proc == "PUSH" || proc == "SCALE" || proc == "HANGLE" || proc == "SWIM")
				if(!Hostile(GetController(clnk), GetOwner()) ) {
					// Nicht erreichbar?
					if(GetY(clnk) < GetY() - 7) if(!PathFree(GetX(), GetY(), GetX(), GetY(clnk)) ) continue;
					if(GetY(clnk) > GetY() + 7) if(!PathFree(GetX(), GetY() + 16, GetX(), GetY(clnk)) ) continue;
					// Priorität prüfen (Cursor hat Priorität, sonst nächster Clonk)
					if(!best)
						best = clnk;
					else if(GetCursor(GetController(clnk)) == clnk)
						best = clnk;
					else if(GetCursor(GetController(best)) != best)
						if(ObjectDistance(clnk) < ObjectDistance(best))
							best = clnk;
				}
		}
	}
	return best;
}


private func SetMoveTo(iPos) {
	if (NoEnergy()) return(1);
	SetAction("Ride"); 
	var iDir = 0;
	if (iPos > GetY()) iDir = +3;
	if (iPos < GetY()) iDir = -3;
	SetCommand(this(),"MoveTo", 0, GetX(), Max(iPos, RangeTop) + iDir);
	SetYDir(0);
	return(1);
}

private func Halt() {
	if (!Elevator()) { SetAction("Idle"); return; }
	SetAction("Wait");
	SetCommand(this,"None");
	SetComDir(COMD_Stop);
	SetYDir(0);
	if (GetY() > RangeBottom) UpdateRangeBottom();
	return(1);
}

private func Drilling() {
	// Steuermann verloren? Suche schiebenden Clonk selbst wenn er ein Fahrzeug im Fahrstuhlkorb schiebt (->Lore)
	var elev = Elevator();
	if (!elev) return Halt();
	if (!FindObject2(Find_InRect(-12,-12,24,24), Find_Action("Push")))
		return Halt();
	// Energieverbrauch
	if (ObjectCount(ENRG)) {
		if (EnergyCheck(30000, elev)) {
			DoEnergy(-30000, elev, true);
		} else {
			Halt();
		}
	}
	return(1);
}

private func Waiting() {
	// Dauerbetrieb: selbständig losfahren
	if (AutoMode == 1)
		SelfMovement();
	return(1);
}

private func SelfMovement() {
	// Selbständig rauf- bzw. runterfahren
	if (AutoMoveDir) SetMoveTo(RangeTop);
	if (!AutoMoveDir) SetMoveTo(LandscapeHeight() - 12);
	AutoMoveDir = !AutoMoveDir;
	return(1);
}


/* Steuerung */

protected func ControlCommand(strCommand, oTarget, iTx, iTy) {
	if (!Elevator()) return(0);
	if (Hostile(GetController(oTarget), GetOwner())) return(0);
	// Bewegungskommando vertikal
	if (strCommand == "MoveTo")
		if (Inside(iTx, GetX()-15, GetX()+15))
			return(SetMoveTo(iTy));
	// Andere Kommandos nicht auswerten
	return(0);
}

protected func ControlCommandFinished() {
	Halt();
	return(1);
}

private func DoControlDown(pObj) { 
	if (!Elevator()) return(0);
	if (NoEnergy()) return(0);
	Sound("Click");
	if (Hostile(GetController(pObj), GetOwner())) return(0);
	SetMoveTo(LandscapeHeight() - 12);
	return 1;
}

private func DoControlDownReleased(object pObj) {
	Halt();
	return 1;
}

private func DoControlUp(pObj) {
	if (!Elevator()) return(0);
	if (NoEnergy()) return(0);
	Sound("Click");
	if (Hostile(GetController(pObj), GetOwner())) return(0);
	SetMoveTo(RangeTop);
	return 1;
}

private func DoControlUpReleased(object pObj) {
	Halt();
	return 1;
}

private func DoControlStop(pObj) {
	if (!Elevator()) return(0);
	Sound("Click");
	if (Hostile(GetController(pObj), GetOwner())) return(0);
	Halt();
	return 1;
}

private func DoControlDig(pObj) {
	if (!Elevator()) return(0);
	if (NoEnergy()) return(0);
	Sound("Click");
	if (Hostile(GetController(pObj), GetOwner())) return(0);
	SetCommand(this,"None");
	SetAction("Drill");
	SetComDir(COMD_Down);
	return 1;
}

private func DoControlDigReleased(pObj) {
	Halt();
	return 1;
}

public func DoControlAuto(pObj) {
	// Kein Fahrstuhl
	if (!Elevator()) return(0);
	// Der Clonk soll uns erst anfassen
	if ((GetAction(pObj) != "Push") || (GetActionTarget(0, pObj) != this))
	{
		SetCommand(pObj, "None");
		AddCommand(pObj, "Call", this, 0,0,0,0, "DoControlAuto");
		AddCommand(pObj, "Grab", this);
		return(0);
	}
	// Ein Geräusch machen
	Sound("Click");
	// Feindliche Clonks können den Modus nicht ändern
	if (Hostile(GetController(pObj), GetOwner())) return(0);
	// Modus ändern
	AutoMode = !AutoMode;
	// Meldung ausgeben
	Message(GetAutoModeString(AutoMode), this);
}        


/* Kontakt */

protected func ContactBottom() {
	Halt();
	UpdateRangeBottom();
	return(1);
}

protected func ContactTop() {
	Halt();
	return(1);             
}

/* Initialisierung */

protected func Initialize() {
	SetAction("Wait");
	UpdateRangeTop();
	UpdateRangeBottom();
	return inherited();
}

private func UpdateRangeTop() { 
	// set Local(8) for backwards compatibility
	Local(8) = RangeTop = GetY();
	UpdateTransferZone();
}

private func UpdateRangeBottom()
{
	var x = GetX(), y = GetY() + 13;
	while(PathFree(x, y, x, y + 5) && y < LandscapeHeight()) y += 5;
	// set Local(9) for backwards compatibility
	RangeBottom = Local(9) = Max(Local(9), y+5);
	UpdateTransferZone();
}

/* Status */

private func Passenger() {
	// Clonk finden
	var clnk = FindObject(0, -12,-13,24,20, OCF_CrewMember,0,0,NoContainer());
	if(!clnk) return (0);
	// steht wirklich auf dem Fahrstuhl?
	if(GetProcedure(clnk) != "WALK") return (0);
	return (clnk);
}

private func Elevator() {
	if(!oElev || GetActionTarget(0, oElev) != this())
		oElev = FindObject(0,0,0,0,0,0,"LiftCase",this());
	return(oElev);
}

private func NoEnergy() {
	if (EnergyCheck(25000,Elevator())) return(0);
	Halt();
	//Sound("ElevatorNoEnergy");
	return(1);
}

public func IsElevator() { return(1); }

/* Transfer */

protected func UpdateTransferZone() { 
	if (!Elevator()) return(SetTransferZone());
	SetTransferZone(-12, GetY(oElev) - GetY(), 24, RangeBottom - GetY(oElev) + 12);
	return(1);
}

protected func ControlTransfer(pObj, iTx, iTy)  {
	// Fahrstuhl gehört dem Gegner? Dann nicht.
	if (Hostile(GetController(pObj), GetOwner())) return(0);

	// Durchgang am Boden des Fahrstuhlschachts: nicht warten
	if (Inside(GetY(pObj) - RangeBottom, -10, +10))
		if (Inside(iTy - RangeBottom, -10, +10))
			return(0);

	// Maximale Obergrenze Zielposition durch Fahrstuhl
	if (!Elevator()) return(0);
	iTy = Max(iTy, GetY(oElev) + 15);

	// Fahrstuhl an Zielposition: erledigt
	if (Inside(iTy - GetY(), -5, +5))
		return(0);

	// Fahrstuhl fährt: warten
	if (GetAction() != "Wait")
		return(1);

	// Fahrstuhl noch nicht beim Clonk: warten
	if (!Inside(GetY() - GetY(pObj), -15, +15))
		return(1);

	// Fahrstuhl nur in der Mitte anfassen
	if (!Inside(GetX() - GetX(pObj), -5, +5))
		return(AddCommand(pObj,"MoveTo",this()));

	// Fahrstuhl anfassen
	if (!InLiquid(pObj) && GetAction(pObj) != "Push" || GetActionTarget(0, pObj) != this())
		return(AddCommand(pObj,"Grab",this()));
	
	// Fahrstuhl zur Zielposition steuern
	return(SetMoveTo(iTy));
}


/* Zerstörung: Reduzieren des Fahrstuhls für Wiederaufbau/Reparatur */

func Destruction() { if(Elevator()) ObjectCall(Elevator(),"RemoveCase"); }

/* Bei Laden von alten Spielständen: Locals zurückkopieren */

func ImportSettings() {
	AutoMode = Local(0);
	AutoMoveDir = Local(7);
	RangeTop = Local(8);
	RangeBottom = Local(9);
}

/* Steuerung */

public func ControlDownSingle(pObj) { 
	[$TxtDown$|Image=ECS1:0|Method=Classic]       
	if(!GetPlrJumpAndRunControl(GetController(pObj))) {
		if (!DoControlDown(pObj)) {
			return inherited(pObj); 
		}
	}
	return 1;
}

public func ControlDown(pObj) {
	[$TxtDown$|Image=ECS1:0|Method=JumpAndRun]
	if(GetPlrJumpAndRunControl(GetController(pObj))) {
		if (!DoControlDown(pObj)) {
			return inherited(pObj);
		}
	}

	return 1;
}

public func ControlDownReleased(pObj) {
	DoControlDownReleased(pObj);
}

public func ControlUp(pObj) { 
	[$TxtUp$|Image=ECS1:1]      
	if (!DoControlUp(pObj)) {
		return inherited(pObj);
	}
	
	return 1; 
}

public func ControlUpReleased(pObj) {
	DoControlUpReleased(pObj);
}

public func ControlDig(pObj) {
	[$TxtDrillshaft$|Image=ECS1:2|Method=JumpAndRun]
	if(GetPlrJumpAndRunControl(GetController(pObj))) { 
		if (!DoControlDig(pObj)) {
			return inherited(pObj);
		}
	}
	return 1; 
}

public func ControlDigDouble(pObj)  {
	[$TxtDrillshaft$|Image=ECS1:2|Method=Classic|Condition=NoContext]
	if(!GetPlrJumpAndRunControl(GetController(pObj))) {
		if (!DoControlDig(pObj)) {
			return inherited(pObj);
		}
	}
	return 1; 
}

public func ControlDigReleased(pObj) {
	DoControlDigReleased(pObj);
}

public func ControlLeft(pObj)  {
	[$TxtStop$|Image=ECS1:3|Method=Classic]
	if (GetAction() != "Wait" && !GetPlrJumpAndRunControl(GetController(pObj)))
	{
		DoControlStop(pObj);
		SetComDir(COMD_Stop, pObj);
	} 
	else
	{
		if(!inherited(pObj))
			if(GetPlrJumpAndRunControl(GetController(pObj)))
			{
				AddCommand(pObj, "Call", this, 0,0,0,0, "ContMoveLeft");
				AddCommand(pObj, "UnGrab");
			}
	}

	return(1);
}

public func ControlRight(pObj) {
	[$TxtStop$|Image=ECS1:3|Method=Classic]
	if (GetAction() != "Wait" && !GetPlrJumpAndRunControl(GetController(pObj)) )
	{
		DoControlStop(pObj);
		SetComDir(COMD_Stop, pObj);
	} 
	else
	{
		if(!inherited(pObj))
			if(GetPlrJumpAndRunControl(GetController(pObj)))
			{
				AddCommand(pObj, "Call", this, 0,0,0,0, "ContMoveRight");
				AddCommand(pObj, "UnGrab");
			}
	}
	return(1);
}

public func ContMoveLeft(object caller) { caller->SetComDir(COMD_Left); }
public func ContMoveRight(object caller) { caller->SetComDir(COMD_Right); }

public func ControlThrow(pObj) {
	[$TxtPutGet$|Image=ECS1:4]
	return inherited(pObj);
}

public func ContextPermanentModeTurnOn(pObj)  {
	[$TxtPermanentModeTurnOn$|Image=ECS1:5|Condition=IsNotInPermanentMode|Desc=$TxtPermanentModeDesc$]
	DoControlAuto(pObj);
}
 
public func ContextPermanentModeTurnOff(pObj)  {
	[$TxtPermanentModeTurnOff$|Image=ECS1:5|Condition=IsInPermanentMode|Desc=$TxtPermanentModeDesc$]
	DoControlAuto(pObj);
}
 
protected func NoContext() {
	return(false);
}

private func GetAutoModeString(iAutoMode) {
	if (iAutoMode) 
		return("$TxtPermanentmodeon$");
	else
		return("$TxtPermanentmodeoff$");
}

public func IsInPermanentMode() {
	return(AutoMode);
}
	
public func IsNotInPermanentMode() {
	return(!AutoMode);
}
