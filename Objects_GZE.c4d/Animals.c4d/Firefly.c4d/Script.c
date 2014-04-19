/*-- Firefly --*/

#strict 2

local startPositionX, startPositionY;

protected func Initialize() {
	SetAction("Fly");
	SetPhase(Random(6));

	var lightColour = RGBa(220, 255, 180, 110);
	var light = AddLightAmbience(RandomX(8,15),this);
	var color = light->ChangeColor(lightColour);

	FadeIn(this, 0, false);
}

private func Flying() {
	var xdir,ydir;

	var fleeFromObj = FindObject2(Find_Distance(50), Find_Category(C4D_Object), Find_OCF(OCF_HitSpeed1), Find_NoContainer());
	if (fleeFromObj != 0) {
		xdir = BoundBy(GetX() - GetX(fleeFromObj), -1, 1);
		ydir = BoundBy(GetY() - GetY(fleeFromObj), -1, 1);
		if (xdir == 0) xdir = Random(2) * 2 - 1;
		if (ydir == 0) ydir = Random(2) * 2 - 1;
		xdir = RandomX(5 * xdir, 10 * xdir);
		ydir = RandomX(5 * ydir, 10 * ydir);

		SetSpeed(xdir, ydir);

		return;
	}

	if (Random(4)) {
		return;
	}

	xdir = Random(13) - 6;
	ydir = Random(9) - 4;

	if (startPositionX+startPositionY>0 && Distance(startPositionX, startPositionY, GetX(), GetY()) > RoamingDistance()) {
		xdir = BoundBy(startPositionX - GetX(), -1, 1);
		ydir = BoundBy(startPositionY - GetY(), -1, 1);
		xdir = RandomX(xdir, 6 * xdir);
		ydir = RandomX(ydir, 6 * ydir);
	}

	SetSpeed(xdir, ydir);
}

private func RoamingDistance() {
	return 40;
}

protected func Check() {
	// Verschüttet oder im Wasser?: Sterben
	if (GBackSemiSolid()) {
		return Death();
	}
}

public func UpdateTime() {
	
	if (IsDawn() || IsDay()) {
		FadeOut(this, 255, false, Random(3));
	}
}


public func CatchBlow() { RemoveObject(); }
public func Damage()	{ RemoveObject(); }
protected func Death()	{ RemoveObject(); }
