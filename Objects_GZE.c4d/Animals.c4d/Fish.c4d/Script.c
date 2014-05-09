/*-- Fisch --*/

#strict 2
#include ANIM

local Bait; // Verfolgter K�der

public func IsPossessible() { return 1; }
func GetAnimalPlacementMaterial() { return Material("Water"); }

/* Initialisierung */

protected func Initialize() { if(GetAction() == "Idle") return(Birth()); }

/* TimerCall mit KI-Steuerung */

private func Activity() 
{
	// Die KI-Steuerung wird bei Besessenheit nicht gebraucht
	if (GetEffect("PossessionSpell", this())) return 0;

	if (GetAction() == "Walk") WalkDir();
	
	// Schwimmt gerade zu einem K�der
	if (Bait) {
		// K�der taugt nix mehr...
		if (GetAction(Bait) != "Bait") {
			Bait = 0;
		// ...ansonsten weiter verfolgen und nicht ablenken lassen
		} else {
			return 1;
		}
	}
		
	var pBait;
	// Nur wenn der Fisch ausgewachsen ist
	if (GetCon()==100 && !Random(5)) {
		 // Ein K�der in der N�he?
		 if (pBait = FindObject( 0, -250,-250,500,500, OCF_InLiquid, "Bait")) {
			 // Je nach K�derqualit�t...
			if (Random(100) < pBait->~IsBait()) {
				 // ...hinschwimmen
				 SetCommand(this(), "Follow", pBait);
				 // Und K�der merken
				 Bait = pBait;
			}
		}
	}
	// Fortpflanzung (sollte eigentlich laichen...)
	if (!Random(ReproductionRate())) {
		Reproduction();
	}
	
	// Vor Bedrohungen fl�chten
	var found_threat;
	if (!Contained() && InLiquid())  {
		// Keine Regelobjekte, Luftblasen etc.
		for (var threat in FindObjects(Find_Distance(100),
			Find_Category(C4D_Living | C4D_Object | C4D_Vehicle | C4D_Structure),
			Find_Not(Find_Category(C4D_Background | C4D_Parallax)), Find_Not(Find_ID(FXU1)),
			// Keine toten Clonks
			Find_Or(Find_Not(Find_Func("IsClonk")), Find_OCF(OCF_Alive)),
			Sort_Distance()))
		{
			// Bedrohung bewegt sich nicht?
			if (Inside(GetXDir(threat), -2, +2) && Inside(GetYDir(threat), -3, +3)) continue;
			// Kontakt zum Boden?
			if (GetContact(threat, -1, 8)) continue;
			// Bedrohung nicht im Wasser oder in einem Container -> N�chstes Objekt
			if (!InLiquid(threat) || Contained(threat)) continue;
			// Keine unsichtbaren Objekte
			if (GetVisibility(threat)) continue;
			// Sind ziemlich naiv und schwimmen nicht vor anderen Tieren weg
			if (!GetAlive(threat) || threat->~IsClonk()) {
				found_threat = true;
				var xdist = GetX(threat) - GetX();
				var ydist = GetY(threat) - GetY();
				var axdist = Abs(xdist);
				var aydist = Abs(ydist);
				var xsign = xdist / axdist;
				var ysign = ydist / aydist;
				var fleex = GetX() - xsign * (1500 / BoundBy(axdist, 20, 80)); // 20..80 -> 70..15
				var fleey = GetY() - ysign * (1000 / BoundBy(aydist, 20, 80)); // 20..80 -> 50..10
				SetCommand(this(), "MoveTo", 0, fleex, fleey, 0, true);
			}
		}
	}
	
	// Bewegung zum Fischturm (hat hoehere Prioritaet als Gefahren) (Tiefsee)
	var fish_tower = FindObject(FSTW, -250, -250, 500, 500, OCF_Fullcon );
	// nicht das U-Boot steuern wenn darin befindlich :D
	if(!Contained()) 
		// Fischturm gefunden
		if(fish_tower)
		{
			SetCommand(this(), "MoveTo", 0, GetX(fish_tower) - 150 + Random(300), GetY(fish_tower) - 150 + Random(300) );
			found_threat = true;
		}

	// Wenn keine Bedrohung mehr oder nicht im Wasser aufh�ren zu fliehen
	if (!InLiquid() || Contained() || (!found_threat && GetCommand() == "MoveTo")) 
		SetCommand(this(), "None");
	
	// Schwimmverhalten
	if (!GBackLiquid(0, -8) && GetAction() != "Walk") return(SetComDir(COMD_Down));
	if (Random(2)) return 1;
	if (GetAction() != "Swim") return 1;
	if (!Random(10)) return(SetComDir(COMD_Up));
	if (!Random(10)) return(SetComDir(COMD_Down));

	// Umdrehen
	if (Random(2)) return(TurnRight());
	return(TurnLeft());
}

private func WalkDir() {
	SetComDir(COMD_Left);
	if (Random(2)) SetComDir(COMD_Right);
	return 1;
}

/* Kontakte */

protected func ContactLeft() {
	// Die KI-Steuerung wird bei Besessenheit nicht gebraucht
	if (GetEffect("PossessionSpell", this())) return 0;

	return TurnRight();
}

protected func ContactRight() {
	// Die KI-Steuerung wird bei Besessenheit nicht gebraucht
	if (GetEffect("PossessionSpell", this())) return 0;

	return TurnLeft();
}

protected func ContactTop() {
	// Die KI-Steuerung wird bei Besessenheit nicht gebraucht
	if (GetEffect("PossessionSpell", this())) return 0;

	SetComDir(COMD_Down);
	return 1;
}

protected func ContactBottom() {
	// Die KI-Steuerung wird bei Besessenheit nicht gebraucht
	if (GetEffect("PossessionSpell", this())) return 0;

	if (GetAction() != "Walk") SetComDir(COMD_Up);
	if (Random(10)) SetComDir(COMD_Right);
	if (Random(10)) SetComDir(COMD_Left);
	return 1;
}

/* Aktionen */

private func TurnRight() {
	if (Stuck() || (GetAction() != "Walk" && GetAction() != "Swim")) return 0;
	if (GetXDir() < 0) SetXDir(0);
	SetDir(DIR_Right);
	SetComDir(COMD_Right);
	return 1;
}

private func TurnLeft() {
	if (Stuck() || (GetAction() != "Walk" && GetAction() != "Swim")) return 0;
	if (GetXDir() > 0) SetXDir(0);
	SetDir(DIR_Left);
	SetComDir(COMD_Left);
	return 1;
}

public func Entrance(container)  {
	// Damit der Fisch nicht aus U-Booten flieht und so.
	SetCommand(this(), "None");
}


/* Einwirkungen */

public func Activate(object pClonk) {
	[$TxtEmbowel$|Image=KNFE]
	if (pClonk)
		{
		// Aquaclonks (und damit Hydroclonks) k�nnen Fische direkt essen
		if (pClonk->~IsAquaClonk()) return(Eat(pClonk));
		// Andere versuchen zu zerlegen (lebende Fische im Inventar d�rften ohnehin selten sein)
		return(ObjectSetAction(pClonk, "Embowel", this()));
		}
	return 1;
}

public func Eat(object pByObject) {
	pByObject->~Feed(50);
	RemoveObject();
	return 1;
}

public func Cook() {
	ChangeDef(CFSH);
	SetAlive(0);
	return 1;
}

protected func Death() {
	SetDir(0);
	ChangeDef(DFSH);
	SetAction("Dead");
	return 1;
}

/* Vermehrung */

private func SpecialReprodCond() {
	return(GetAction() == "Swim");
}

public func Birth() {
	var pEnv;
	if (pEnv=FindObject(CLFS)) pEnv->CLFS::Colorize(this());
	else SetColorDw(RGB(255,255,255));
	SetAction("Swim");
	SetComDir(COMD_Left);
	if(Random(2)) SetComDir(COMD_Right);
}

public func RejectEntrance(pContainer) {
	// Aquaclonks/Hydroclonks k�nnen ungeachtet der Regel immer einsammeln
	// (In Tiefsee sind Fische die einzige M�glichkeit, Energie aufzuladen!)
	if (pContainer->~IsAquaClonk()) return 0;
	// Fischt�rme sollten auch immer funktionieren
	if (pContainer->~IsFishTower()) return 0;
	// ANIM-Definition (Einsammelbare Tiere)
	return(_inherited(pContainer, ...));
}

/* Steuerung durch Besessenheit */

protected func ControlCommand(szCommand, pTarget, iTx, iTy) {
	// Bewegungskommando
	if (szCommand == "MoveTo") {
		return(SetCommand(this(),szCommand, pTarget, iTx, iTy));
	}
	return 0;
}

protected func ContainedLeft(object caller) {
	[$TxtMovement$]
	SetCommand(this(),"None");
	if(!GetPlrJumpAndRunControl(caller->GetController()))
		TurnLeft();
	return 1;
}

protected func ContainedRight(object caller) {
	[$TxtMovement$]
	SetCommand(this(),"None");
	if(!GetPlrJumpAndRunControl(caller->GetController()))
		TurnRight();
	return 1;
}

protected func ContainedUp(object caller) {
	[$TxtMovement$]
	SetCommand(this(),"None");
	if(!GetPlrJumpAndRunControl(caller->GetController()))
		SetComDir(COMD_Up);
	return 1;
}

protected func ContainedDown(object caller) {
	[$TxtMovement$]
	SetCommand(this(),"None");
	if(Contained()) return SetCommand(this, "Exit");
	if(!GetPlrJumpAndRunControl(caller->GetController()))
		SetComDir(COMD_Down);
	return 1;
}

/* JumpAndRun-Steuerung */

private func ClearDir(bool fX) {
	if(fX && GetXDir()) {
		if(GetXDir() > 0) SetXDir(Max(GetXDir() - 2, 0));
		else SetXDir(Min(GetXDir() + 2, 0));
	}
	if(!fX && GetYDir()) {
		if(GetYDir() > 0) SetYDir(Max(GetYDir() - 2, 0));
		else SetYDir(Min(GetYDir() + 2, 0));
	}
}

public func ContainedUpdate(object self, int comdir) {
	SetComDir(comdir);
	ClearScheduleCall(this(), "ClearDir");
	if(comdir == COMD_Down || comdir == COMD_Up) ScheduleCall(this(), "ClearDir", 1, (Abs(GetXDir())+1)/2, true);
	if(comdir == COMD_Left || comdir == COMD_Right) ScheduleCall(this(), "ClearDir", 1, (Abs(GetYDir())+1)/2, false);

	return 1;
}

protected func ContainedThrow() {
	[$TxtDrop$]
	var iEffectNumber, pSorcerer;
	if (iEffectNumber = GetEffect("PossessionSpell", this())) {
		if (pSorcerer = EffectVar(0, this(), iEffectNumber)) {
			if (pSorcerer->Contents()) pSorcerer->Contents()->Exit(0,0,6);
			AddEffect("IntCollectionDelay", this(), 1, 70);
		}
	}
	return 1;
}

protected func ContainedDigDouble() {
	[$TxtLeave$]
	RemoveEffect("PossessionSpell", this());
	return 1;
}

/* Aufwertungszauberkombo: Mit Fisch wird der Clonk zum Aquaclonk */
public func GetRevaluationCombo(object pClonk) { return(ACLK); }


/* Zerlegen nach Clonktyp */

protected func GetCustomComponents(object pClonk) {
	if (pClonk) {
		// Jedem seine Extraw�rste
		if (pClonk->~IsTrapper() || pClonk->~IsIndian()) return ([FSHM, FSHM, FSHB]);
		if (pClonk->~IsInuk()) return([MEAT, FAT1, FSHB]);
		//if (pClonk->~IsJungleClonk()) return([MEAT, FAT1, FSHB]); // Jungelclonk kann (noch) nicht zerlegen
	}
}


/* Kranke Tiere (Arktis) */

public func Sick() { ChangeDef(QFSH);	Birth(); }
