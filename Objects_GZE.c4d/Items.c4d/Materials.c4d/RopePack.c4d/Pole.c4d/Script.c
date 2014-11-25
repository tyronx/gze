/*-- Pfahl --*/

#strict 2

local fLocked;

public func ControlDigDouble(pClonk)
{
  [$TxtLock$]
  if(Stuck())
  {
    var i;
    while(Stuck() && i++ < 12)
      SetPosition(GetX(), GetY() - 1);
  }
  else
  {
    if(!GBackSolid(0, 3))
      SetPosition(GetX(), GetY() + 3);
  }
  Sound("Connect");
  return 1;
}

protected func Hit() { if(!Stuck()) return Sound("WoodHit*"); }

public func IsLocked() { return Stuck(); }

// Nicht an die Dampflok koppeln lassen
public func NoPull() { return 1; }
  
public func GetVertexToConnect() { return 2; }
