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
func TreeType() { return "evergreen"; } // deciduous or evergreen (decides whether to drop leaves or not)
func ShakeStrength() { return 3; } // decides how wiggly the tree is when shaking

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
	if (Abs(GetWind()) < 50 + MotionThreshold && !GetEffect("Shake", this)) SetAction("Still");
	if (Abs(GetWind()) > 74 + MotionThreshold && GetActMapVal("Name", "Storm")) SetAction("Storm");
}

private func Storm() {
	if (Abs(GetWind()) < 75 + MotionThreshold  && !GetEffect("Shake", this)) SetAction("Breeze");
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
		

// Called when Clonk hits the tree with his Axe during chop
public func AxeHit(pClonk) {
	Sound("Chop*");
	pClonk->CastParticles("Dust",Random(3)+1,6,-8+16*pClonk->GetDir(),1,10,12);
	var strength = ShakeStrength();
	if (!(pClonk->GetDir())) {
		strength *= -1;
	}
	Shake(strength);
	if (!Random(3)) CastLeafParticles();
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
	while (Stuck() && (++Var(0) < 6))  {
		SetPosition(GetX(), GetY() - 1, this);
	}
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


public func CastLeafParticles() {
	if (OnFire() || TreeType() != "deciduous") {
		return 0;
	}

	var def = GetID();
	var amount = Random(GetCon()/20) + 1;

	for (var i=0; i < amount; i++) {
		var x = GetDefOffset(def, 0) + Random(GetDefWidth(def) - 20) +10;
		var y = GetDefOffset(def, 1) + Random(GetDefHeight(def) - GetDefFireTop(def));

		var rgb = RGB(0,110,0);
		if (Random(2)) {
			rgb = RGB(64,150,64);
		}

		CreateParticle("Leaves", x, y, 0, 1, RandomX(15, 25), rgb);
	}
}



/* Rütteln und Schütteln */

public func Shake(int strength) {
	RemoveEffect("Shake", this);
	AddEffect("Shake", this, 50, 1, this, 0, strength);
	if (!(GetAction()=="Breeze" || GetAction()=="Storm")) {
		if (GetActMapVal("Name", "Storm")) {
			SetAction("Storm");
		} else {
			SetAction("Breeze");
		}
	}
}

public func FxShakeStart(object target, int nr, int temp, int strength) {
	EffectVar(0, target, nr) = BoundBy(strength, -7, 7);
	EffectVar(1, target, nr) = GetR(target); // original rotation
	EffectVar(2, target, nr) = 9-Abs(EffectVar(0, target, nr));
}

public func FxShakeTimer(object target, int nr, int time) {
	if (!(target->IsStanding())) {
		return -1;
	}

	var strength = EffectVar(0, target, nr);

	if (strength == 0) {
		// let the effect linger on a little so that the "Breeze" animation still plays
		EffectVar(3, target, nr)++;
		if (EffectVar(3, target, nr) > 60) {
			return -1;
		}
	}
	
	var rot0 = EffectVar(1, target, nr);
	target->RelSetR(rot0 + Cos(time*EffectVar(2, target, nr)*2, strength * target->GetCon())/100, 0, (3*GetDefHeight(target->GetID())*(target->GetCon())/400));
	
	if (!(time%7) && (strength != 0)) {
		EffectVar(0, target, nr) -= BoundBy(EffectVar(0, target, nr), -1, 1);
	}
}

public func FxShakeEffect(string name) {
	if (name == "Shake") {
		return -2;
	}
}

public func FxShakeAdd(object target, int nr, string name, int timer, int strength) {
	EffectVar(0, target, nr) = BoundBy(EffectVar(0, target, nr) + strength, -7, 7);
	EffectVar(2, target, nr) = 9-Abs(EffectVar(0, target, nr));
}

public func FxShakeStop(object target, int nr, int iReason, bool fTemp) {
 	if (!fTemp) {
 		target->SetR(EffectVar(1, target, nr));
 	}
}



/* Status */

public func IsTree() { return 1; }

public func IsStanding() { return(~GetCategory() & C4D_Vehicle); }

public func IsDeadTree() { return false; } // Überladen von toten Bäumen
