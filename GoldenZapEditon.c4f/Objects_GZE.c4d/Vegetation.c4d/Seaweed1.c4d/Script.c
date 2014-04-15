/*-- Seegras --*/

#strict

protected func CheckEnvironment()
{
  if (GBackLiquid())
    if (GetAction() S= "Dry")
      return(StartWiggle());
  if (!GBackLiquid())
    if (GetAction() S= "Wiggle")
      return(StartDry());
  return(1);
}

private func StartWiggle()
{
  SetAction("Wiggle");
  SetPhase(Random(8));
}
  
private func StartDry()
{
  SetAction("Dry");
}
 
protected func Completion()
{
  StartDry();
  return(1);
}



public func IsHangingVegetation() { return(0); }
public func IsUndergroundVegetation() { return(0); }
public func IsSubmergedVegetation() { return("Water"); }
public func GetVegetationSoil() { return ("Sand"); }
public func GetVegetationRootDepth() { return(4); } 
