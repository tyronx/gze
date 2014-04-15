/*-- Enterhaken --*/

#strict 2

local pRope, iR, iRDir;

func Initialize() { return SetPicture(8, 0, 64, 64); }

func Launch(pByClonk)
{
  SetAction("Idle");
  // Abbrechen, wenn nur ab- oder irgendwo hineingelegt
  if(GetPlrDownDouble(GetOwner(pByClonk)) || Contained(pByClonk) || GetAction(pByClonk) == "Push")
    return 0;
  // Drehung für kurz geworfenen Haken
  iR = 150 + 170 * GetDir(pByClonk);
  iRDir = -12 + 24 * GetDir(pByClonk);
  // Rest nur für "Erstwurf"
  if(pRope) return 1;
  var pPulley = CreateObject(CL5P, 0, 0, GetOwner());
  pRope = CreateObject(CK5P, 0, 0, GetOwner());
  pRope->ConnectObjects(pPulley, this);
  pPulley->SetRope(pRope, this);
  pPulley->Enter(pByClonk);
  LocalN("fNoPickUp_1", pRope) = 1;
  LocalN("iPull", pPulley) = 7;
  SetPicture(72, 0, 64, 64);
  iR = 90 * GetDir(pByClonk);
  if(!GBackLiquid() && GetAction(pByClonk) != "Swim")
  {
    // Drehung für normal weit geworfenen Haken
    iRDir = -6 + 12 * GetDir(pByClonk);
    // Haken weit werfen
    SetXDir((GetXDir() * 15) / 10);
    SetYDir((GetYDir() * 25) / 10);
  }
  Sound("Catapult");
  return 1;
}

func RopeBreak()
{                                     
  pRope = 0;
  if(GetActionTarget(0, pRope)) RemoveObject(GetActionTarget(0, pRope));
  SetPicture(8, 0, 64, 64);
  SetAction("Idle");
  return 1;
}

func Hit()
{
  Sound("WoodHit*");
  if(!pRope) return 0;
  SetXDir(); SetYDir(); iRDir = 0;
  return 1;
}

// Wird bei versuchter Aufnahme durch Clonk aufgerufen
// Ermöglicht unabhängig des Inventarlimits das Zurücksetzen zu normalem Enterhaken
public func JoinPack(pClonk)
{
  for(var pObj in FindObjects(Find_Container(pClonk), Find_ID(CL5P)))
    if(GetActionTarget(0, pRope) == pObj || GetActionTarget(1, pRope) == pObj)
      return Reset(pClonk);
  return 0;
}

func Reset(pBy)
{
  if(GetActionTarget(0, pRope)) RemoveObject(GetActionTarget(0, pRope));
  if(pRope) RemoveObject(pRope);
  if(Contained() != pBy) Enter(pBy);
  SetAction("Idle");
  SetPicture(8, 0, 64, 64);
  Sound("ArrowHit");
  return 1;
}

private func Rotate()
{
  var xoff = 0, yoff = 0;
  iR -= iRDir / 3;
  if(GetContact(this, -1)) iRDir = 0;
  var fsin = Sin(iR, 1000), fcos = Cos(iR, 1000);
  SetObjDrawTransform (
    +fcos, +fsin, (1000-fcos)*xoff - fsin*yoff,
    -fsin, +fcos, (1000-fcos)*yoff + fsin*xoff);
}

func Entrance(pContainer)
{
  SetAction("Idle");
  // Enterhaken mit passender Seilrolle zusammenfügen
  for(var pObj in FindObjects(Find_Container(pContainer), Find_ID(CL5P)))
    if(GetActionTarget(0, pRope) == pObj || GetActionTarget(1, pRope) == pObj)
      Reset(pContainer);
}

func ContactLeft() { if(GetAction() != "PickUp" && pRope) return SetAction("AttachLeft"); }
func ContactRight() { if(GetAction() != "PickUp" && pRope) return SetAction("AttachRight"); }
func ContactBottom() { if(GetAction() != "PickUp" && pRope) return SetAction("AttachDown"); }
func ContactTop() { if(GetAction() != "PickUp" && pRope) return SetAction("AttachUp"); }
public func IsHook() { return 1; }
public func IsAnvilProduct() { return 1; }
