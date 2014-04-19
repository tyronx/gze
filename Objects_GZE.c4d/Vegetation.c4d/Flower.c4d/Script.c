/*-- Flower --*/

#strict 2
#include GRAS

protected func Initialize(obj) { 
	SetAction("Flower");
	SetPhase(Random(3));
	MoveBehindTrees();
}

public func GetVegetationSoil() { return ("Grass"); }
public func GetVegetationIsFlower() { return (1); }
public func GetVegetationRootDepth() { return(2); } 
// Auto rotate the vegetation to fit the landscape between these angles
public func GetVegetationRotates() { return [-30, 30]; }
