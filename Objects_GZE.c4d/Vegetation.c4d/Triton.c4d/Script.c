/*-- Tritonmuschel --*/

#strict

protected func Initialize() { 
  SetEntrance(1); 
}

public func IsSubmergedVegetation() { return("Water"); }
public func GetVegetationSoil() { return ("Sand"); }
public func GetVegetationRootDepth() { return(4); } 
public func GetVegetationPlacementBalance() { return(50); } // place only 50% as much