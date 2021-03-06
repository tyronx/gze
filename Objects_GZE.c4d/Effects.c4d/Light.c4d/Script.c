/*-- Licht --*/

#strict 2

local iColor, bAmbience, iF;

//Licht initialisieren, Parameter setzen, etc.
protected func Init(int iSize, int iColor, object pTarget, string cGraphics) {
//Werte
  if(!iSize)
    iSize = GetCon();  
  if(!pTarget)
    pTarget = GetActionTarget();

  SetOwner(GetController(pTarget));

//Setzen und so
  SetAction("Attach",pTarget);
  SetPosition(GetX(pTarget),GetY(pTarget));
  ChangeColor(iColor);
  ChangeSize(iSize);
  ChangeGraphics(cGraphics);
}

//Wenn das Licht kein Ziel mehr hat -> weg damit.
protected func AttachTargetLost() {
  RemoveObject();
}

//Licht ausschalten
public func TurnOff() { SetVisibility(VIS_None); }

//Licht einschalten
public func TurnOn() { SetVisibility(VIS_All); }

// gr��e des Lichts �ndern
public func ChangeSize(int iNewSize) { 
  var alphamod, sizemod;
  CalcLight(alphamod, sizemod);

  iF = iNewSize;
  SetCon(iF*sizemod/100);
}

// Licht updaten...
protected func Update() {
  ChangeSize(iF);
  ChangeColor(iColor);
}

//Farbe des Lichts �ndern
public func ChangeColor(int iNewColor) {
  
  iColor = iNewColor;
  
  //Wenn keine dunkelheit ist
  if(!IsDark() || !GetDarkness()) {
    //Hintergrundlichter ausblenden
    if(bAmbience) {
      iNewColor = RGBa(255,255,255,255);
      SetClrModulation(iNewColor);
      return iNewColor;
    }
  }  
  
  var alphamod, sizemod;
  CalcLight(alphamod, sizemod);
  
  var r,g,b,a;
  SplitRGBaValue(iColor,r,g,b,a);
  iNewColor = RGBa(r,g,b,Min(a+60+alphamod,255));
  
  SetClrModulation(iNewColor);
  
  return(iNewColor);
}

//Grafik �ndern
public func ChangeGraphics(string cNewGraphics) {
  SetGraphics(cNewGraphics, 0, GetID());
}

//festlegen, dass es ein Licht ist.
public func IsLight() { return(1); }
public func NoWarp() { return(true); }

/*--  Funktionen zur Lichterzeugung  --*/

//Hilfsfunktion
func CreateLight(id ID, int iSize, int iColor, object pTarget, string sGraphics) {
  var light = CreateObject(ID, 0, 0, GetController(pTarget));
  light->Init(iSize, iColor, pTarget, sGraphics);
  
  return(light);
}

//erzeugt ein Licht mit Gr��e und Farbe und h�ngt es an pTarget
func AddLight(int iSize, int iColor, object pTarget) {
  if(!pTarget)
    if(!(pTarget = this))
      return;
  
  return(CreateLight(LIGH, iSize, iColor, pTarget));
}

func AddLightHalf(int iSize, int iColor, object pTarget) {
  if(!pTarget)
    if(!(pTarget = this()))
      return;
  
  return(CreateLight(LIGH, iSize, iColor, pTarget, "Half"));
}

func AddLightCone(int iSize, int iColor, object pTarget) {
  if(!pTarget)
    if(!(pTarget = this()))
      return;
  
  return(CreateLight(LGHC, iSize, iColor, pTarget));
}

//Ambience-Light, Umgebungslicht, nicht sichtbar wenn keine Dunkelheit herrscht.
func AddLightAmbience(int iSize, object pTarget, string cGraphics) {
  if(!pTarget)
    if(!(pTarget = this()))
      return;
      
  var light = CreateObject(LIGH,0,0,GetController(pTarget));
  light->LocalN("bAmbience") = true;
  light->Init(iSize*5, RGBa(255,255,255,50), pTarget, cGraphics);
  
  return(light);
}

func AddBuildingLight() {
	AddVertex(); // mittiger Vertex erstellen, damit das Licht zentriert wird
	var pLight = AddLight(GetDefWidth(GetID()) * 2, RGB(200, 200, 255));
	pLight -> LocalN("bAmbience") = true;
	pLight -> SetActionData(256*2 + GetVertexNum() - 1);
	pLight -> Update();
	return pLight;
}

//Lichtblitz!
func AddLightFlash(int iSize, int iX, int iY, int iColor) {
  var alphamod, sizemod;
  CalcLight(alphamod, sizemod);
  
  var r,g,b,a;
  SplitRGBaValue(iColor,r,g,b,a);
  iColor = RGBa(r,g,b,Min(a+alphamod,255));

  CreateParticle("LightFlash",iX,iY,0,0,iSize*5*sizemod/100,iColor);
}
