#strict 2


// var dwRGB = HSL(RandomX(0,38),RandomX(127,255),RandomX(64,160));
//  SetColorDw(dwRGB);
// add 13 hue

// 0 = unripe (green)    increase Con, turing very slightly green-yellowish
// 80 = ripening (turning from green to yellow to red)
// 100-300 = ripe
// 280 = falls off the tree
// 301 = starting to spoil (going red to brown)
// 325 = not collectible anymore
// 330 = decrease con
// 350 = brown and spoiled
// 366 = con = 0, removeobject
local growth;
local collected;

func GrowBerry() {
	// Picked berries last forever
	if (collected) return(0);
	
	growth++;
	SetGrowthStage();
}

func SetGrowthStage() {
	if (growth >= 330) {
		SetCon(100 - (growth*3 - 660));
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
	if (growth >= 100 && growth <= 300) {
		SetColorDw(HSL(0, 250, 128));
	}

	if (growth > 280 && GetCategory() == 1) {
		Drop();
	}
	
	if (growth > 300) {
		var chng = growth - 300;
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
	SetGrowthStage();
}

// Dropped from tree
public func Drop() {
	SetActionTargets();
	SetCategory(16);
}

// Clonks pick up only ripe and not spoiling fruits
protected func RejectEntrance() {
	return growth < 100 || growth > 325;
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
