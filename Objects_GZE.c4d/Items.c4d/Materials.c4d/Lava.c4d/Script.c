/*--- Lava ---*/

#strict 2

local strMat;

protected func Initialize() {
  SetAction("Evaporate");
  return(1);
}
  
protected func Hit() {
	if(!strMat) strMat="Lava";
	CastPXS(strMat, GetCon()/2, 35);
	RemoveObject();
	return(1);
}

private func SmokeTrail() {
	Smoke(0, 0, 5);
	Smoke(0, -5, Random(7));
	if (GetActTime() > 75) Hit();
	return(1);
}

public func SetMaterial(material) {
	strMat = material;
}
  
