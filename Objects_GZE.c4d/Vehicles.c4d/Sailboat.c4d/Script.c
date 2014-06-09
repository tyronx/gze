/*-- Ankerfähiges Segelboot --*/

#strict 2

local pRope, pAnchor;
local iPull;

/* Initialisierung */

protected func Initialize() { SetAction("Sailing"); }

/* Steuerung */

protected func ControlLeft(object clonk)
{
  [$TxtLeft$|Image=SLB1:0]
  if(!GetPlrCoreJumpAndRunControl(clonk->GetController()))
    SetComDir(COMD_Left);

  return(1);
}

protected func ControlRight(object clonk)
{
  [$TxtRight$|Image=SLB1:1]
  if(!GetPlrCoreJumpAndRunControl(clonk->GetController()))
    SetComDir(COMD_Right);

  return(1);
}

protected func ControlDigDouble()
{
  [$TxtAnchor$|Image=SLB1:3]
  if(!pAnchor)
    return LowerAnchor();
  if(iPull >= 0)
  {
    iPull = -3;
    pAnchor->~Raise();
    return 1;
  }
  if(iPull < 0)
    iPull = 4;
  return 1;
}

protected func ControlUpdate(object clonk, int comdir)
{
  if(comdir == COMD_UpLeft || comdir == COMD_Left || comdir == COMD_DownLeft)
    SetComDir(COMD_Left);
  else if(comdir == COMD_UpRight || comdir == COMD_Right || comdir == COMD_DownRight)
    SetComDir(COMD_Right);
  else
    SetComDir(COMD_Stop);
}

protected func ControlUpDouble()
{
  [$TxtLand$|Image=SLB1:2]
  if(GetAction() == "Sailing")
    SetAction("RaiseSail");
  return(1);
}

protected func ControlCommand(string szCommand, object pTarget, int iX, int iY)
{
  // Bewegungskommando (nur links/rechts auswerten)
  if(szCommand == "MoveTo")
    return(Command2Control(iX,iY));
}

private func Command2Control(int iX, int iY)
{
  // nur X wird ausgewertet
  if(iX>GetX()) SetComDir(COMD_Right);
  if(iX<GetX()) SetComDir(COMD_Left);
  return(1);
}

/* Segeln */

private func Wind2Sail()
{
  // Kein Steuermann
  if (!FindObject(0,0,0,0,0,0,"Push",this()))
    SetComDir(COMD_None);
  // Geschwindigkeit und Bewegung
  var wind_push = BoundBy(GetWind() / 4, -10, +10);
  var xdir = 7 + wind_push / 5; /* windabhängige X-Dir */
  if (GetComDir() == COMD_None)
    SetXDir(wind_push);
  if (GetComDir() == COMD_Left)
    SetXDir(-xdir);
  if (GetComDir() == COMD_Right)
    SetXDir(+xdir);
}

private func LandOn()
{
  ChangeDef(SLBT);
  SetComDir(COMD_None, this);
  SetAction("JustLanded", this);
}

private func LowerAnchor()
{
  iPull = 4;
  SetAction("RaiseSail2");
  pRope = CreateObject(CK5P, 0, 0, GetOwner());
  pAnchor = CreateObject(CZ5P, -14, 32, GetOwner());
  pRope->ConnectObjects(pAnchor, this);
  LocalN("fNoPickUp_0", pRope) = 1;
  LocalN("fNoPickUp_1", pRope) = 1;
  LocalN("pBoat", pAnchor) = this;
  pAnchor->SetR(35);
  return 1;
}

private func RaiseAnchor()
{
  if(!pAnchor) return 0;
  pAnchor->RemoveObject();
  SetAction("LowerSail");
  return 1;
}

public func RopeBreak()
{
  if(pAnchor) pAnchor->RemoveObject();
  SetAction("LowerSail");
  return 0;
}

private func AnchorHit()
{
  if(iPull > 0)
    iPull=0;
  if(iPull < 0)
    pAnchor->~Raise();
  return 1;
}

func RopeAskChangeLength(iLength)
{
  if(iPull > 0 && iLength >= 0 && LocalN("iLength", pRope) < 500)
    return 2;
  if(iPull < 0 && pRope && pAnchor && LocalN("iLength", pRope) <= 10)
    RaiseAnchor();
  if(iPull <= 0 && iLength >= 10)
    return 1;
  if(iPull == 0 && iLength <= -5)
    return -1;
  if(iPull < 0 && iLength <= 4)
    return -1;
  return 0;
}

public func GetVertexToConnect() { return 3; }
public func IsBoat() { return 1; }

/* Sounds */

private func SoundSailDown() { Sound("SailDown"); }
private func SoundSailUp()   { Sound("SailUp");   }
