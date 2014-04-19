/*-- Grass --*/

#strict 2

global func FireflyFrequency() { return 4; } 

local spawned_fireflies;

public func UpdateTime(hours, hoursOld) {
	if(IsNight() && !spawned_fireflies) {

		spawned_fireflies = true; // nicht alle Gräser müssen Glühwürmchen erstellen!

		if( GetTemperature()>10 && !Random(FireflyFrequency())) {
			var count = RandomX(1, 2);
			var dist = 30;

			for(var i = 0; i < count; i++) {
				var firefly = CreateObject(FFLY, RandomX(-dist,dist), RandomX(-dist,dist), -1);
				firefly->LocalN("startPositionX") = GetX();
				firefly->LocalN("startPositionY") = GetY() - 15;
			}

		}
	}
	else if (IsDay() && spawned_fireflies)
	{
		spawned_fireflies = false;
	}
}

protected func Initialize() { 
	// Zufällige Form	
	SetAction("Grass");
	SetPhase(Random(2));
	if (!Random(20)) SetPhase(2);
	// Zufällige Richtung
	if (Random(2)) SetDir(DIR_Right);
 
	// Gras bleibt hinter Bäumen
	MoveBehindTrees();
}

private func MoveBehindTrees() {
	for (var obj in FindObjects(Find_OCF(OCF_Chop), Find_NoContainer(), Find_Category(C4D_StaticBack), Find_AtPoint())) {
		if (obj->~IsTree()) {
			//Log("put %v behind tree", GetID());
			SetObjectOrder(obj, this, 1); 
		}
	}
}

// Bei nahen Explosionen
public func OnShockwaveHit(iLevel,iX,iY) {
	var con=(40*GetCon())/100;
	iLevel=40+iLevel/2;
	/*if (Material("Grass") != -1) {
		//CastPXSX("Earth", (GetCon()*RandomX(60, 100))/100, RandomX(20, 80),  0, -1, -90, 45);
		
		//CastPXSX(string mat,int am,int lev,int x,int y,int angs,int angw)
		CastPXSX("Grass", (GetCon()*RandomX(60, 100))/100, RandomX(iLevel/2, iLevel),  RandomX(-con/2,con/2), -1, -90, 45);
	} else {*/
		for(var cnt=0;cnt<15+Random(10);cnt++) {
			CreateParticle("GrassBlade",RandomX(-con/2,con/2),-1,RandomX(-iLevel/3 ,iLevel/3),RandomX(-2*iLevel/3,-iLevel/3),30+Random(30),RGB(255,255,255),0,0);
		}
	//}
	return(RemoveObject());
}

// Kann immer von Schockwellen getroffen werden
public func CanBeHitByShockwaves(){return(true);}
public func BlastObjectsShockwaveCheck(){return(true);}


public func IsHangingVegetation() { return(0); }
public func IsUndergroundVegetation() { return(0); }
public func IsSubmergedVegetation() { return(0); }
public func GetVegetationSoil() { return ("Grass"); }
public func GetVegetationIsFlower() { return (GetPhase()==2); }

// Place twice as much - you can have never enough grass :D
public func GetVegetationPlacementBalance() { return(200); }
// How deep are the plants roots? (places item x pixel inside the soil)
public func GetVegetationRootDepth() { return(8); }
// Auto rotate the vegetation to fit the landscape between these angles
public func GetVegetationRotates() { return [-35, 35]; }
// Randomly size the vegetation between these 2 cons. This value is capped at 1-100, so you can e.g. set it to [50,200] to increase the chance of getting big vegetation
public func GetVegetationCons() { return [50, 80]; }
