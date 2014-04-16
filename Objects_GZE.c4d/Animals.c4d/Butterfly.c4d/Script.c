/*-- Schmetterling --*/

// NewSyntax by Miniwipf
#strict

////////////////////////////// Script //////////////////////////////

/*-- Engine-Calls --*/

// Initialisierung
protected func Initialize()
{
  // Fliegen! :D
  SetAction("Fly");
  MoveToTarget();
  //SetClrModulation(RGBa(RandomX(0,255),RandomX(0,255),RandomX(0,255)),this());
  return();
}

// TimerCall
protected func Activity()
{
  // Unterwasser :( Nach oben fliegen
  if (InLiquid()) return(SetComDir(COMD_Up()));
  // Sitzt: warten
  if (SEqual(GetAction(), "Sit")) return();
  // Neues Ziel
  if (!GetCommand() && !Random(5)) MoveToTarget();
  return();
}

/*-- Bewegung (Action-Calls) --*/

protected func Flying()
{
  // Richtung anpassen
  if (GetXDir() > 0) SetDir(DIR_Right());
  if (GetXDir() < 0) SetDir(DIR_Left());  
  // Aktion ändern
  if (!Random(3)) SetAction("Flutter");
  return();
}

protected func Fluttering()
{
  // Richtung anpassen
  if (GetXDir() > 0) SetDir(DIR_Right());
  if (GetXDir() < 0) SetDir(DIR_Left());   
  // Aktion ändern
  if (!Random(7)) SetAction("Fly");
  return();
}

/*-- Kontakt (CNAT) --*/

protected func ContactTop()
{
  SetCommand(this(), "None");
  SetComDir(COMD_Down());
}

protected func ContactBottom()
{
  SetCommand(this(), "None");
  SetComDir(COMD_Up());
  return();
}

protected func ContactLeft()
{
  SetCommand(this(), "None");
  SetComDir(COMD_Right());
}

protected func ContactRight()
{
  SetCommand(this(), "None");
  SetComDir(COMD_Left());
}

/*-- Aktionen --*/

private func SitDown()
{
  // Absitzen und nichts tun
  SetXDir();
  SetYDir();
  SetComDir(COMD_Stop());
  SetAction("Sit");
  SetCommand(0, "None");
  return();
}
  
private func TakeOff()
{
  SetComDir(COMD_Up());
  return();
}

private func MoveToTarget()
{
  var iX = Random(LandscapeWidth());
  var iY = Random(GetHorizonHeight(iX)-60) + 30;
  SetCommand(this(), "MoveTo", 0, iX, iY,,, 1);
  return();
}
  
protected func Death()
{
  AddEffect("FadeOut", this(), 101, 1);
  return(1);
}

private func GetHorizonHeight(int iY)
{
  while (iY < LandscapeHeight() && !GBackSemiSolid(0, iY))
    iY += 10;
  return(iY);
}

global func FxFadeOutStart(target, number) {
  if(!target) return(-1);
  EffectVar(0, target, number) = 0;
}

global func FxFadeOutTimer(target, number,byR,byG,byB) {
  EffectVar(0, target, number)++;
  SetClrModulation(RGBa(255,255,255, EffectVar(0, target, number)), target);
  if(EffectVar(0, target, number) >= 255) return(-1);
}

global func FxFadeOutStop(target) {
  RemoveObject(target);
}
