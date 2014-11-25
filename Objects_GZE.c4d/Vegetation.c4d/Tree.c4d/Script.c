/*-- Baumsteuerung --*/

#strict 2

local MotionThreshold;
local MotionOffset;
// Chopdown time now based by amount of chops applied to tree
local chops;
local zapNest;

/* Überladbare Konstanten */

func ReproductionAreaSize() { return 400; } // Größe des Bereichs, in dem neue Bäume dieser Art entstehen können
func ReproductionRate()     { return 500; } // Die Chance, dass in einem Timerintervall eine Vermehrung stattfindet
func MaxTreeCount()         { return 40; } // Maximale Baumanzahl im Umkreis
func TreeStrength() { return 150; } // Dicke des Stammes
func CanHouseZapNest() { return 0; }
func CreateZapNestVertex() { return 0; }
func ZapNestVertexAttach() { return 4; }

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
	if (SetAction("Initialize")) {
		// Ausrichtung
		SetDir(Random(2));
	}
	if (CanHouseZapNest()) {
		ScheduleCall(this(), "PutZapnest", 2);
		
	}
	return 0;
}
	
private func Initializing() {
	// Bis zum Aktionszeitoffset gewartet: los geht's
	if (GetPhase() == MotionOffset) {
		SetAction("Still");
	}
	// Noch warten
	return 1;  
}


public func PutZapnest() {
	if (GetCon() != 100 || GetR() != 0) return(0);
	
	if (!Random(10) && GetCon()>50 && !FindObject2(Find_ID(ZAPT), Find_InRect(-40,-40,80,80))) {
		zapNest = CreateObject(ZAPT);
		
		CreateZapNestVertex();

		zapNest->SetAction("HangOnTree", this());
		zapNest->SetActionData(ZapNestVertexAttach());
	}
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
	if (zapNest) {
		zapNest->SetAction("Idle");
	}
	
	ScheduleCall(this(), "TurnToWood", 60);
	// Bereits gefällt
	if (!IsStanding()) return 0;
	// Kategorie ändern
	SetAction("Idle");
	SetCategory(C4D_Vehicle);
	// Aus der Erde lösen
	while (Stuck() && (++Var(0) < 6)) {}
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

	var x1 = -Sin(GetR(), - wdt/2)/10;
	var y1 = Cos(GetR(), - hgt/2)/10;
	var x2 = -Sin(GetR(), wdt/2)/10;
	var y2 = Cos(GetR(), hgt/2)/10;
	
	CreateObjectLine(treematerial, quantity, [x1, y1, x2, y2]);
	
	RemoveObject();
}



/* Status */

public func IsTree() { return 1; }

public func IsStanding() { return(~GetCategory() & C4D_Vehicle); }

public func IsDeadTree() { return false; } // Überladen von toten Bäumen
