/*-- Anker --*/

#strict 2

local pBoat;

public func Raise()
{
  if(GetAction() == "AttachGround")
    SetAction("Idle");
  return 0;
}

protected func Hit()
{
  if(pBoat)
  {
    SetAction("AttachGround");
    pBoat->~AnchorHit(this);
  }
  return 0;
}

public func GetVertexToConnect() { return 1; }
public func Entrance(pContainer) { SetAction("Idle"); }
public func IsAnchor() { return 1; }
public func IsAnvilProduct() { return 1; }
