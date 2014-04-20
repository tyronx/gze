/*-- Busch --*/

#strict 2

#include TREE

private func ReproductionAreaSize() { return(400); }
private func ReproductionRate()     { return(500); }
private func MaxTreeCount()         { return(15); }
private func TreeStrength()  { return(40); }
private func FruitRate()     { return(40); }
private func FruitID()       { return(WMPF); }
private func MaxFruitCount() { return(3); }

// Start out with a few fruits
func Construction() {
	ScheduleCall(this, "StartingFruits", 1);
	return inherited();
}

private func StartingFruits() {
	var fruits = RandomX(0, (MaxFruitCount() * GetCon()) / 100);
	while (fruits-- > 0) {
		GrowFruit(RandomX(10,150));
	}
}

// Check every 350 frames for fruit placement
private func Seed()	 {
	// Fruitrate also dependent on how big the bush is
	if (IsStanding() && !Random(FruitRate() + (100 - GetCon()))) {
		GrowFruit();
	}
	return _inherited();
}

// Returns array of fruits growing on this bush
public func GetFruits() {
	return FindObjects(Find_ID(FruitID()), Find_ActionTarget(this));
}

// Fruits drop when chopped down
public func ChopDown() {
	for (var pFruit in GetFruits()) {
		pFruit->~Drop();
	}
	return _inherited();
}

// Fruits drop & burn when incinerated
public func Incineration() {
	for (var pFruit in GetFruits()) {
		pFruit->~Drop();
		pFruit->Incinerate();
	}
	// statt BurnTo in der Defcore
	ChangeDef(BUSB);
}

/* Funktionen zum Platzieren der Früchte */

private func FruitRectangle(&iWidth, &iHeight, &iOffset) {
  var iFireTop = GetDefFireTop(GetID());
  iWidth =   GetDefWidth(GetID())*GetCon()/100;
  iHeight =  GetDefHeight(GetID())*GetCon()/100;
  iHeight -= iFireTop;

  // Frucht soll nicht "überhängen"
  iWidth  -= iWidth/5;
  iHeight -= iHeight/5;

  var iOffset = -iFireTop/2;
}

public func GrowFruit(int iGrowth) {
	var fruitcount = GetLength(GetFruits());
	
	if (!iGrowth) {
		iGrowth = 10;
	}

	if (fruitcount < MaxFruitCount()) {
		var iWidth, iHeight, iOffset;
		FruitRectangle(iWidth, iHeight, iOffset);

		var iAngle = RandomX(-80, 80);
		
		// Placed in a circle
		return	PlaceFruit(
			FruitID(),
			Cos(iAngle,RandomX(-iWidth/2,iWidth/2)),
			Sin(iAngle,RandomX(-iHeight/2,iHeight/2)),
			iGrowth
		);
	}
	
	// Already full tree
	return(0);
}


private func PlaceFruit(id idFruit, int iX, int iY, int iGrowth) {
	return CreateObject(idFruit,iX,iY,-1)->~Place(this(), iGrowth);
}


public func GetVegetationSoil() { return ("Grass"); }
// How deep are the plants roots? (places item x pixel inside the soil)
public func GetVegetationRootDepth() { return(10); }