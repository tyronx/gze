/*-- Stalaktit --*/

#strict

local phase;

func Initialize() {
  SetAction("Hanging");
  SetPhase(phase=Random(4));
}

func Check() {
  var clnk;
  if(GetAction()S="Falling") return(Falling());
    
  while(clnk=FindObject(0,-25,0,50,100,OCF_CrewMember(),0,0,0,clnk)) 
    if(GetAlive(clnk) && !Random(2)) { 
      Sound("Earthquake"); 
      SetAction("Falling");
      SetPhase(phase);
      RemoveVertex(0);
      RemoveVertex(0);
    }
}

func Falling() {
  var clnk;

  while(clnk=FindObject(0,-7,-19,14,50,OCF_CrewMember(),0,0,0,clnk)) {
    DoEnergy(RandomX(-5,0),clnk);
  }

  if (GetActTime() > 700) Hit();
}

func Hit() {
  var obj;
  if(GetAction()ne"Falling") return(1);
  Sound("RockBreak*");
  Sound("Blast2",0,0,50);
  // links oben
  ObjectSetAction(obj=CreateObject(_STP,-4,-4,-1),"Exist");
  SetPhase(phase*2,obj);
  SetRDir(Random(20)-10,obj);
  SetSpeed(Random(30)-15,-Random(35)+10,obj);
  // rechts oben
  ObjectSetAction(obj=CreateObject(_STP,3,-4,-1),"Exist");
  SetPhase(phase*2+1,obj);
  SetRDir(Random(20)-10,obj);
  SetSpeed(Random(30)-15,-Random(35)+10,obj);
  // unten
  ObjectSetAction(obj=CreateObject(_STP,0,12,-1),"LameExist");
  SetPhase(phase,obj);
  SetRDir(Random(20)-10,obj);
  SetSpeed(Random(10)-15,-Random(35)+10,obj);
	
  RemoveObject();
}


public func IsHangingVegetation() { return(1); }
public func IsUndergroundVegetation() { return(1); }
public func IsSubmergedVegetation() { return(0); }
public func GetVegetationSoil() { return ("Rock"); }
// How deep are the plants roots? (places item x pixel inside the soil)
public func GetVegetationRootDepth() { return (5); }