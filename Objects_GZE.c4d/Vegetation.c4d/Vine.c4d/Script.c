/*-- Liane --*/

#strict 2

protected func Construction() {
  // Zufälliges Aussehen
  SetAction("Vine");
  SetPhase(Random(10));

  ScheduleCall(this(), "GrowVine", 1);
}

public func GrowVine() {
	
}

public func GetVineLength() {
  return (GetCon() * GetDefCoreVal("Height") / 100);
}

public func IsHangingVegetation() { return(1); }
public func IsUndergroundVegetation() { return(0); }
public func IsSubmergedVegetation() { return(0); }
public func GetVegetationSoil() { return ("Grass"); }

// How deep are the plants roots? (places item x pixel inside the soil)
public func GetVegetationRootDepth() { return (5); } 
// Randomly size the vegetation between these 2 cons. This value is capped at 1-100, so you can e.g. set it to [50,200] to increase the chance of getting big vegetation
public func GetVegetationCons() { return [50, 200]; }