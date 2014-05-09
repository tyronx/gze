/*-- Zap --*/

#strict 2
local target;

local peaceful;
local nestobj;

local lifetime;

protected func Construction() {
	lifetime = RandomX(80,160);
}

func CollectHoneyTo(mynestobj) {
	peaceful =1;
	nestobj=mynestobj;
	
	for (var objplant in FindObjects(Sort_Distance(), Find_Category(C4D_SelectVegetation()))) {
		if (objplant->~GetVegetationIsFlower()) {
			SetCommand (this(), "MoveTo", objplant);
		}
	}
	
}


private func Flying() { // wird alle 3 Frames aufgerufen
  var xdir,ydir;
//Log("ttl %d", lifetime);
  // nichts tun
  if(Random(4)) return(1);
  // Geschwindigkeit zuf�llig bestimmen
  xdir=Random(13)-6;
  ydir=Random(9)-4;
  // Verfolgt der Zap jemanden? Geschwindigkeit abstimmen
  xdir+=BoundBy((GetX(target)-GetX()),-11,11);
  ydir+=BoundBy((GetY(target)-GetY())/3,-7,7);
  if(GBackLiquid(xdir,ydir)) return(SetSpeed());
  SetSpeed(xdir,ydir);
}

protected func Check() { // wird alle 18Frames aufgerufen
  var obj;

  // Versch�ttet oder im Wasser?: Sterben
  if(GBackSemiSolid()) return(Death());
  // einfach so Sterben
  //if(lifetime-- < 0) return(Death());
	if(!Random(160)) return(Death());
  // Summen
  if(!Random(50)) Sound("Bsss");

  // Vom aktuellen Ziel ablassen und jemand anders verfolgen (falls jemand da ist)
  if(!Random(10)) target=FindTarget(200);
  // Einfach nur so vom Ziel ablassen
  if(!Random(30)) target=0; 
  // Das Ziel hat sich in ein Geb�ude gerettet
  if(Contained(target)) target=0; 
  // Vom Ziel ablassen wenn er sich im Wasser versteckt
  if(GBackSemiSolid(GetX(target)-GetX(),GetY(target)-GetY())) target=0;
  // Vom Ziel ablassen weil er nicht direkt erreichbar ist
  if(!Random(5)) if(!PathFree(GetX(),GetY(),GetX(target),GetY(target))) target=0; 

  // stechen
  if(!Random(2))
    if(obj=FindObject(0,1,0,0,0,OCF_Prey,0,0,NoContainer()))
      Sting(obj);
}
  
private func Sting(obj) {
  Punch(obj,3+Random(2));
  Sound("Sting");
  if(!Random(3)) Death();
}

public func FindTarget(maxdist) {  // maximale Distanz zum "Opfer"
  var obj;
  if(peaceful) return;
  // neues Ziel bestimmen falls es nicht zu weit entfernt ist
  if(obj=FindObject(0,0,0,-1,-1,OCF_Prey,0,0,NoContainer()))
    if(ObjectDistance(obj)<maxdist)
      if(!GBackSemiSolid(GetX(obj)-GetX(),GetY(obj)-GetY()))
        return(obj);
}

protected func Initialize() {
  SetAction("Fly");
  SetPhase(Random(6));
}

public func CatchBlow()	{ RemoveObject(); }
public func Damage()	{ RemoveObject(); }
protected func Death()	{ RemoveObject(); }