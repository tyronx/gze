/*-- Shell--*/

#strict 2

func Hit() {
  Sound("MetalHit*");
  return 1;
}

public func IsHangingVegetation() { return 0; }
public func IsUndergroundVegetation() { return 0; }
public func IsSubmergedVegetation() { return "Water"; }
public func GetVegetationSoil() { return "Sand"; }
// How deep are the plants roots? (places item x pixel inside the soil)
public func GetVegetationRootDepth() { return 1; }


func IsAlchemContainer() { return true; }
func AlchemProcessTime() { return 50; }

