/*-- Fish --*/

#strict 2
#include AIBA // A.I. Base

local fullness;

func GetAnimalPlacementMaterial() { return Material("Water"); }

func ActivityInit() { 
	fullness = RandomX(0, 50);
	AddActivities(["AvoidThreats", "Sleep", "Feed", "Swim"]);
}

func ShouldExecuteAvoidThreats() {
}

func ContinueExecuteAvoidThreats() {
}



func ShouldExecuteSleep() {
}

func ContinueExecuteSleep() {
}



func ShouldExecuteFeed() {
	if (Random(2) && fullness > 0) fullness--;
	if (fullness<= 0) {
		
		return 1;
	}
	return 0;
}

func ContinueExecuteFeed() {
	
}


func ShouldExecuteSwim() {
	SetCommand(this, "None");
	return 1;
}

func ContinueExecuteSwim() {
	if (!GetCommand()) {
		var x,y, attempts = 10;
		while (attempts-- > 0) {
			if (GBackLiquid(x += RandomX(-50, 50), y += RandomX(-50, 50))) {
				SetCommand(this, "MoveTo", 0, GetX() + x, GetY() + y, 0,  1);
				break;
			}
		}
	}
}


/* TimerCall mit KI-Steuerung */

private func Activity() 
{
	// Die KI-Steuerung wird bei Besessenheit nicht gebraucht
	if (GetEffect("PossessionSpell", this())) return 0;

	if (GetAction() == "Walk") WalkDir();
	
	// Schwimmt gerade zu einem Köder
	if (Bait) {
		// Köder taugt nix mehr...
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
		 // Ein Köder in der Nähe?
		 if (pBait = FindObject( 0, -250,-250,500,500, OCF_InLiquid, "Bait")) {
			 // Je nach Köderqualität...
			if (Random(100) < pBait->~IsBait()) {
				 // ...hinschwimmen
				 SetCommand(this(), "Follow", pBait);
				 // Und Köder merken
				 Bait = pBait;
			}
		}
	}
	// Fortpflanzung (sollte eigentlich laichen...)
	if (!Random(ReproductionRate())) {
		Reproduction();
	}
	
	// Vor Bedrohungen flüchten
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
			// Bedrohung nicht im Wasser oder in einem Container -> Nächstes Objekt
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

	// Wenn keine Bedrohung mehr oder nicht im Wasser aufhören zu fliehen
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

