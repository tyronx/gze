/*-- Fackel --*/

#strict 2

public func Activate()
{
  if(GetAction() == "Burning") return(Extinguishsh());
  SetAction("Burning");
  return(this() );
}
  
private func Extinguishsh()
{
  SetAction("Idle");
  Sound("Pshshsh");
  return(1);
}

private func StructureCheck()
{
  //if(!FindCastlePart(1, 1) )
 //   RemoveObject();
  return(0);
}

public func CastleChange() { return(StructureCheck() ); }
