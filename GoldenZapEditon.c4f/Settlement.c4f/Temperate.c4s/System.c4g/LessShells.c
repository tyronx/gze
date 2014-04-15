/*-- Because we have little sand in this scenario, we individually balance out the placement of these items here  --*/

#strict
#appendto SHEL

// Place only half as much
func GetVegetationPlacementBalance() { return(50); }

// How deep are the plants roots? (places item x pixel inside the soil)
public func GetVegetationRootDepth() { return(1); }