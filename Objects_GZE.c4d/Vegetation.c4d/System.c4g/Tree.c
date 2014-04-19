#strict 2
#appendto TREE

public func GetVegetationSoil() { return ("Grass"); }
public func GetVegetationRootDepth() { return(10); }
// Auto rotate the vegetation to fit the landscape between these angles
public func GetVegetationRotates() { return [-30, 30]; }
// Randomly size the vegetation between these 2 cons. This value is capped at 1-100, so you can e.g. set it to [50,200] to increase the chance of getting big vegetation
public func GetVegetationCons() { return [5, 200]; }