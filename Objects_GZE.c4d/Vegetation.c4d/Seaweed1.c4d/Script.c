/*-- Seegras --*/

#strict 2
#include TREE

// The deeper the Seagrass, the less green it becomes
func Construction() {
	var y = 0;
	// Why is this happening?
	if (GetCon() == 0) SetCon(100);
	
	while (GetMaterial(0, y) != -1 && y > -300) {
		y -= 30;
	}
	
	var dwRGB = HSL(
		BoundBy(80 + (3*y)/10, 35, 80), 
		BoundBy(180 + y/10, 150, 180),
		BoundBy(150 + y/10, 120, 150)
	);
	
	SetColorDw(dwRGB);
}

protected func CheckEnvironment() {
	if (!Random(17 + (100-GetCon())/2)) {
		CreateObject(FXU1, 0, -3, -1);
	}
	if (GBackLiquid() && GetAction() == "Dry") {
		return(StartWiggle());
	}
	if (!GBackLiquid() && GetAction() == "Wiggle") {
		return(StartDry());
	}
	return 1;
}

private func StartWiggle() {
	SetAction("Wiggle");
	SetPhase(Random(8));
}
  
private func StartDry() {
	SetAction("Dry");
}
 
protected func Completion() {
	StartDry();
	return(1);
}

public func IsHangingVegetation() { return(0); }
public func IsUndergroundVegetation() { return(0); }
public func IsSubmergedVegetation() { return("Water"); }
public func GetVegetationSoil() { return ("Sand"); }
public func GetVegetationRootDepth() { return(4); } 
public func FishFoodQuality() { return(5); } 
