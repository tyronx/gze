/*-- Baumsteuerung --*/

#strict 2

local MotionThreshold;
local MotionOffset;
// Chopdown time now based by amount of chops applied to tree
local chops;

/* Überladbare Konstanten */

private func ReproductionAreaSize() { return 400; } // Größe des Bereichs, in dem neue Bäume dieser Art entstehen können
private func ReproductionRate()     { return 500; } // Die Chance, dass in einem Timerintervall eine Vermehrung stattfindet
private func MaxTreeCount()         { return 40; } // Maximale Baumanzahl im Umkreis

private func TreeStrength() { return 150; } // Dicke des Stammes

public func GetVegetationSoil() { 
	// Failsafe in case grass material is not available
	if (Material("Grass")!=-1) {
		return "Grass"; 
	}
	return "Earth";
}
public func GetVegetationRootDepth() { return 10; }
// Auto rotate the vegetation to fit the landscape between these angles
public func GetVegetationRotates() { return [-30, 30]; }
// Randomly size the vegetation between these 2 cons. This value is capped at 1-100, so you can e.g. set it to [50,200] to increase the chance of getting big vegetation
public func GetVegetationCons() { return [5, 200]; }


/* Initialisierung */

protected func Construction() {
  // Bewegungsschwelle
  MotionThreshold = Random(10);
  // Aktionszeitoffset
  MotionOffset = Random(GetActMapVal("Length", "Breeze"));
  // Verzögerte Initialisierung: Schlägt SetAction fehl, ist es ein alter Baum ohne Animation. Motion-Variablen werden ignoriert.
  if (SetAction("Initialize"))
    {
    // Ausrichtung
    SetDir(Random(2));
    }
  return 0;
}
  
private func Initializing() {
  // Bis zum Aktionszeitoffset gewartet: los geht's
  if (GetPhase() == MotionOffset) 
    SetAction("Still");
  // Noch warten
  return 1;  
}

/* Bewegung (Wind) */  
  
private func Still() {
  if (Abs(GetWind()) > 49 + MotionThreshold) SetAction("Breeze");
}
    
private func Breeze() {
  if (Abs(GetWind()) < 50 + MotionThreshold) SetAction("Still");
}

   
/* Kontext */

public func ContextChop(pClonk) {
  [$TxtChop$|Image=CXCP|Condition=IsStanding]
  AddCommand(pClonk, "Chop", this());
  return 1;
}

/* Fortpflanzung */

private func Seed() {
  return 0;
}

public func Reproduction() {
	// Ist noch Platz für einen Baum?
	var iSize = ReproductionAreaSize();
	var iOffset = iSize / -2;
	if (ObjectCount(0, iOffset, iOffset, iSize, iSize, OCF_Chop)<MaxTreeCount()) {
		// OK, hin damit
		return PlaceVegetation(GetID(this()), iOffset, iOffset, iSize, iSize, 10);
	}
	// Kein Platz ;'(
	return 0;
}
    
/* Schaden */    

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
	// Bereits gefällt
	if (!IsStanding()) return 0;
	// Kategorie ändern
	SetAction("Idle");
	SetCategory(C4D_Vehicle);
	// Aus der Erde lösen
	while (Stuck() && (++Var(0) < 6))
	SetPosition(GetX(), GetY() - 1, this());
	// Umfallen
	SetRDir(+10); if (Random(2)) SetRDir(-10);
	// Geräusch
	if (GetCon() > 50) Sound("TreeDown*");
	// Fertig
	return 1;
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


/* Status */

public func IsTree() { return 1; }

public func IsStanding() { return(~GetCategory() & C4D_Vehicle); }

public func IsDeadTree() { return false; } // Überladen von toten Bäumen
