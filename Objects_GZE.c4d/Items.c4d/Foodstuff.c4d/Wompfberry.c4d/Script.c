#strict 2

// 0 = unripe (green)    increase Con, turing very slightly green-yellowish
// 90 = ripening (turning from green to yellow to red)
// 100-150 = ripe
// 120 = falls off the tree
// 141 = starting to spoil (going red to brown)
// 165 = not collectible anymore
// 170 = decrease con
// 190  = brown and spoiled
// 206 = con = 0, removeobject
local growth;

local collected;

// By default, the wompfberry shall not grow or wither
func Construction() {
	collected = 1;
}

func GrowBerry() {
	// Picked berries last forever
	if (collected) return(0);
	
	growth++;
	
	// Spoil quicker in dropped & in water
	if (GBackLiquid() && GetCategory() == 16) {
		growth += 2;
	}
	
	SetGrowthStage();
}

func SetGrowthStage() {
	if (growth >= 170) {
		SetCon(100 - (growth*3 - 340));
		if (GetCon() < 10) RemoveObject();
	} else {
		SetCon(Min(100, growth));
	}
	
	if (growth <= 90) {
		// go from hue 80 to hue 60  with con 0 - 90
		
		// turn number 0-90 into 0-20
		var huediff = (growth * 10) / 45;
		
		SetColorDw(HSL(80 - huediff, 250, 128));
	}
	if (growth > 90 && growth < 100) {
		// go from hue 60 to hue 0  within con 90-100
	
		// turn 90-100 into 0-10 into 0-60
		var huediff = (growth - 90) * 6;
		
		SetColorDw(HSL(60 - huediff, 250, 128));
	}
	if (growth >= 100 && growth <= 140) {
		SetColorDw(HSL(0, 250, 128));
	}

	if (growth > 120 && GetCategory() == 1) {
		Drop();
	}
	
	if (growth > 140) {
		var chng = growth - 140;
		SetColorDw(HSL(chng/2, 250 - chng, 128 - chng/2));
	}
}

protected func Construction() {
	SetAction("Exist");
	SetDir(Random(4));
}

public func Place(object pTree, int iGrowth) {
	SetCategory(1);
	SetActionTargets(pTree);
	if (!iGrowth) {
		iGrowth=Random(100)+1;
	}
	growth = iGrowth;
	collected = 0;
	SetGrowthStage();
}

// Dropped from tree
public func Drop() {
	SetActionTargets();
	SetCategory(16);
}

// Clonks pick up only ripe and not spoiling fruits
protected func RejectEntrance() {
	return growth < 100 || growth >= 165;
}

// Picked from tree
protected func Entrance() {
	if (GetCategory()==1) {
		Sound("Grab");
		collected=1;
	}
	Drop();
}

// Eat 
public func Activate(object pClonk) {
	[$TxtEat$]
	Eat(pClonk);
	return(1);
}

protected func Eat(object pClonk) {
	pClonk->~Feed(GetFeedValue());
	pClonk->DoBreath(100);
	RemoveObject();
	return(1);
}

public func GetFeedValue() { return(20); }

protected func Hit() { Sound("WoodHit*"); }
