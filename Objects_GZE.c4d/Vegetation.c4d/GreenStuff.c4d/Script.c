/*-- Grünzeug --*/

// #include TREE - nope

#strict 2

protected func Initialize()
  {
  SetAction(Format("Green%d", Random(4)));
  SetR(Random(80)-40);
  return(1);
  }
  
protected func Damage() 
{
  if (GetDamage() < 15) return(0);
  CastObjects(WOOD, 1, 25, 0, -5);
  RemoveObject();
  return(1);
}

public func IsHangingVegetation() { return(0); }
public func IsUndergroundVegetation() { return(0); }
public func IsSubmergedVegetation() { return(0); }
public func GetVegetationSoil() { return ("Grass"); }
// Creates a Treeline. Percentual y-Values
public func GetVegetationLineTop() { return(0); }
public func GetVegetationLineBottom() { return(50); }
