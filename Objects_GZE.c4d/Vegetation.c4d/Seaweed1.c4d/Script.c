/*-- Seegras --*/

#strict 2
#include TREE

func Construction() {
	var dwRGB = HSL(RandomX(38,80),RandomX(150,180),RandomX(120,150));
	SetColorDw(dwRGB);

}

protected func CheckEnvironment() {
	if (!Random(12 + (100-GetCon())/2)) {
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
