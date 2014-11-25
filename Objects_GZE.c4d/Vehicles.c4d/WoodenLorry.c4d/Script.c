/*-- Lore --*/

#strict 2

#include LORY

/* Füllmengenkontrolle */

private func MaxContents() { return 25; }

protected func RejectCollect(id idObj,object pObj)
{
  if(ContentsCount() < MaxContents())
  {
    Sound("WoodHit*");
    return 0;
  }
  if(Contained(pObj)) return Message("$TxtLorryisfull$", this);
  if(Abs(GetXDir(pObj))>6) SetYDir(-5,pObj);
  Sound("WoodHit*");
  return 1;
}

/* Radkontrolle */
protected func CheckWheel()
{
  // Aktion der Geschwindigkeit anpassen
  if (Abs(GetXDir()))
    SetAction("WheelTurn");
  else SetAction("Idle");
  // Drehung der Richtung anpassen
  if(GetXDir()>0) SetDir(1);
  else if(GetXDir()<0) SetDir(0);
}