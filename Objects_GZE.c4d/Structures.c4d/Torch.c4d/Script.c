/*-- Fackel --*/

#strict 2

local light;

func ControlDigDouble(pCaller) {
	CreateContents(PTRC, pCaller);
	RemoveObject(light);
	RemoveObject();
}

public func Place() {
	if(GetAction() == "Burning") return(Extinguishsh());
	
	SetPlrViewRange(150);
	SetAction("Burning");
	var lightColour = RGBa(240, 240, 180, 30);
	light = LIGH->AddLightAmbience(RandomX(40,60),this);
	light->ChangeColor(lightColour);
	
	return(this() );
}

private func Extinguishsh() {
	SetAction("Idle");
	Sound("Pshshsh");
	if (light) RemoveObject(light);
	return(1);
}

private func StructureCheck() {
  //if(!FindCastlePart(1, 1) )
 //   RemoveObject();
  return(0);
}

public func CastleChange() { return(StructureCheck() ); }
