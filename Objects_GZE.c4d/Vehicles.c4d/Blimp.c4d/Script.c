/*-- Luftschiff --*/

#strict 2
#include VETR

local turn_end_dir;

// Needs to be relative Coordinates x,y,wdt,hgt  
func IsVehicleTransporter() { return 1; }
func VehicleGrabArea() { return [-22, -10, 44, 40]; }
func VehicleHoldingArea() { return [-16, 0,32,30]; }


// Example func to determine when to relay clonk controls to the held vehicle
func CanRelayControl(controltype, pClonk) { 
	if (controltype == "ControlLeft" || controltype == "ControlRight") {
		return GetAction() == "WaterLand" || GetAction() == "FloatIdle";
	}
	return 1; 
}


/* Drehung */

private func TurnStart()
{
  // Alle Clonks die das Luftschiff anfassen registrieren
  var controllers = FindObjects(Find_Action("Push"), Find_ActionTarget(this()));
  AddEffect("IntTurn", this(), 1, 1, this(), 0, controllers);
}

protected func FxIntTurnStart(object target, int number, int temp, array controllers)
{
  if(temp) return(0);

  // Alle luftschiffanfassenden Clonks
  EffectVar(0, target, number) = controllers;
  // Deren initiale Position auf dem Luftschiff...
  EffectVar(1, target, number) = CreateArray(GetLength(controllers));
  // ... die hier ermittelt wird
  for(var i = 0; i < GetLength(controllers); ++ i)
    EffectVar(1, target, number)[i] = controllers[i]->GetX() - GetX();
}

protected func FxIntTurnTimer(object target, int number, int time)
{
  // Laenge der Turn-Action bestimmen
  var delay = GetActMapVal("Delay", "Turn");
  var phases = GetActMapVal("Length", "Turn");
  // Schon vorbei
  if(time == delay * phases) return(-1);
  // Eine Sinus-Kurve von -90 bis +90 Grad hernehmen um die neue Position
  // der Clonks auf dem Luftschiff zu bestimmen.
  var sin_phase = (time * 180 / (phases * delay)) - 90;
  var pos_phase = Sin(sin_phase, 50) + 50;
  // Clonkpositionen anpassen
  var controller_count = GetLength(EffectVar(0, target, number));
  for(var i = 0; i < controller_count; ++ i)
  {
    var controller = EffectVar(0, target, number)[i];
    var pos = EffectVar(1, target, number)[i];

    // Koennte sein, dass ein Controller verloren geht
    if(!controller) continue;
    // Koennte auch sein, dass er kein Controller mehr ist
    if(controller->GetAction() != "Push" || controller->GetActionTarget() != this())
    {
      // Dann nichts mehr mit ihm anstellen, selbst wenn er das Luftschiff
      // wieder anfaesst - er koennte es an einer anderen Position tun und
      // dann beamen wir ihn komisch auf dem Luftschiff rum. Lieber nicht.
      EffectVar(0, target, number)[i] = 0;
      continue;
    }

    // Dir anpassen wenn Haelfte der Turn-Action vorbei
    if(time == delay * phases / 2)
      controller->SetDir(GetDir());

    // Neue Position ermitteln (Sinusfoermig um den Luftschiff-Mittelpunkt)
    var pos_t = -pos * 2;
    controller->SetPosition(GetX() + pos - (pos*2 * pos_phase / 100), controller->GetY());
  }
}

protected func FxIntTurnStop(object target, int number, bool temp)
{
  if(temp) return(0);
  var length = GetLength(EffectVar(0, target, number));

  // Nochmal alle Controller durchgehen
  for(var i = 0; i < length; ++ i)
  {
    var object = EffectVar(0, target, number)[i];
    var pos = EffectVar(1, target, number)[i];

    // Und an fertige Position tun, zur Sicherheit, wenn es noch Controller sind
    if(!object) continue;
    if(object->GetAction() == "Push" && object->GetActionTarget() == this())
      object->SetPosition(GetX() - pos, object->GetY());
  }
}

private func TurnEnd()
{
  if(turn_end_dir != -1)
  {
    SetDirection(turn_end_dir);
    turn_end_dir = -1;
  }
}

/* Steuerung */

private func DoDirection(int comdir)
{
  var cur_comdir = GetComDir();
  if(GetAction() == "FloatIdle") cur_comdir = COMD_Stop;

  cur_comdir = ComDirTransform(cur_comdir, comdir);
  //while(cur_comdir == COMD_Stop)
  //  cur_comdir = ComDirTransform(cur_comdir, comdir);

  SetDirection(cur_comdir);
}

private func ClearDir(bool fX)
{
  if(fX && GetXDir())
  {
    if(GetXDir() > 0) SetXDir(GetXDir() - 1);
    else SetXDir(GetXDir() + 1);
  }
  if(!fX && GetYDir())
  {
    if(GetYDir() > 0) SetYDir(GetYDir() - 1);
    else SetYDir(GetYDir() + 1);
  }
}

private func SetDirection(int comdir)
{
  // Richtungsaenderung nach oben/unten geht auch mit "Turn", aber eine
  // ComDir-Aenderung, die wieder eine Turn-Action erfordern wuerde muss
  // warten, bis die jetzige Turn-Action fertig ist.
  if(GetAction() == "Turn")
  {
    turn_end_dir = comdir;
    if(comdir == COMD_Stop || (ComDirLike(comdir, COMD_Right) && GetDir() == DIR_Left) || (ComDirLike(comdir, COMD_Left) && GetDir() == DIR_Right))
    {
      return(0);
    }
  }

  // ComDir uebernehmen
  SetComDir(comdir);

  // Angebrachte Action setzen
  if(GetAction() != "Turn" && GetAction() !=  "WaterLaunch")
  {
    if(comdir != COMD_Stop && GetAction() != "FloatPropel")
      SetAction("FloatPropel");
    if(comdir == COMD_Stop && GetAction() != "FloatIdle")
      SetAction("FloatIdle");
  }

  // Vorherige ClearDirs wegtun
  ClearScheduleCall(this(), "ClearDir");
  // Wenn Bewegung in nur eine Richtung, dann Bewegung in die andere
  // Richtung aufgeben (macht FLOAT-Prozedur nicht automagisch)
  if(comdir == COMD_Down || comdir == COMD_Up)
    ScheduleCall(this(), "ClearDir", 1, Abs(GetXDir()), true);
  if(comdir == COMD_Left || comdir == COMD_Right || comdir == COMD_Stop)
  {
    ScheduleCall(this(), "ClearDir", 1, Abs(GetYDir()), false);

    // Bei Bewegung nur in X-Richtung ein wenig schaukeln
    if(!GetEffect("IntWindYDir", this()))
      AddEffect("IntWindYDir", this(), 1, 10, this());
  }
  else
  {
    if(GetEffect("IntWindYDir", this()))
      RemoveEffect("IntWindYDir", this());
  }

  if(ComDirLike(comdir, COMD_Right) && GetDir() == DIR_Left)
  {
    SetDir(DIR_Right);
    SetAction("Turn");
  }

  if(ComDirLike(comdir, COMD_Left) && GetDir() == DIR_Right)
  {
    SetDir(DIR_Left);
    SetAction("Turn");
  }

  // Beim Landen auf COMD_Down lassen, sonst hopst das Luftschiff schnell
  // wieder in die Luft
  if(comdir == COMD_Stop && GBackSolid(0, GetObjHeight()/2+2))
    SetComDir(COMD_Down);
}

protected func ControlUpdate(object controller, int comdir, bool dig, bool throw)
{
  SetDirection(comdir);
}

public func ControlThrow(pObj) 
{
  [$TxtPutGet$|Image=ECS1:4]
  return inherited(pObj);
}

protected func ControlUp(object controller)      // hoch
{
  [$TxtAscend$]
  ClearCommand();
  if (GetAction()=="DropOff") return(0);
  if (GetAction()=="WaterLand") SetAction("WaterLaunch");

  if(!GetPlrJumpAndRunControl(controller->GetController()))
    DoDirection(COMD_Up);
}

protected func ControlDownSingle(object controller)  // runter
{
  [$TxtDescend$|Method=Classic]
  if (GetAction()=="DropOff") return(0);
  if(!GetPlrJumpAndRunControl(controller->GetController()))
  {
    ClearCommand();
    DoDirection(COMD_Down);
  }
}

protected func ControlDown(object controller)
{
  [$TxtDescend$|Method=JumpAndRun]
  // Lediglich fuer das Kommandosymbol bei JnR-Steuerung
}

protected func ControlRight(object controller)       // rechts
{
  [$TxtTurnright$]
  ClearCommand();
  if (!inherited(controller)) {
	  if(!GetPlrJumpAndRunControl(controller->GetController()))
	  {
		if (GetAction()=="DropOff") return(0);
		if (GetAction()=="Turn") return(0);
		SetAction("FloatPropel");
		DoDirection(COMD_Right);
	  }
   } else {
	  return 1;
  }
  return 0;

}

protected func ControlLeft(object controller)        // links
{
  [$TxtTurnleft$]
  ClearCommand();

	if (!inherited(controller)) {
	  if(!GetPlrJumpAndRunControl(controller->GetController()))
	  {
		if (GetAction()=="DropOff") return(0);
		if (GetAction()=="Turn") return(0);
		SetAction("FloatPropel");
		DoDirection(COMD_Left);
	  }
  } else {
	  return 1;
  }
  return 0;
}

protected func ControlCommand(string szCommand,object pTarget,int iX,int iY)
{
  // Pilot läßt los
  if (szCommand == "UnGrab") {
	ClearCommand();
	NoPilotCheck();
    return 0;
  }
  // Bewegungskommando vom Piloten
  if (szCommand == "MoveTo")
    return(SetCommand(this(),szCommand,pTarget,iX,iY));
  return(0);
}

private func ClearCommand() { SetCommand(this(),"None"); }

/* Initialisierung */

protected func Initialize()
{
  turn_end_dir = -1;
  FloatIdleDown();
	return inherited();
}

/* Aktivität */

protected func FxIntWindXDirTimer()
{
  if(GetAction() != "FloatIdle" && GetAction() != "WaterLand") return(-1);
  if(GetContact(0, -1, 8)) return;
  SetXDir(BoundBy(GetWind()/4, Max(GetXDir() - 2, -10), Min(GetXDir() + 2, 10)));
}

protected func FxIntWindYDirTimer()
{
  if(GBackSolid(0, GetObjHeight()/2+2)) return(-1);
  SetYDir(BoundBy(GetYDir() + RandomX(-GetYDir()) - 1 + Random(3), Max(GetYDir() - 1, -3), Min(GetYDir() + 1, 3)));
}

private func FloatProcess()
{
  if(!GetEffect("IntWindXDir", this()))
    AddEffect("IntWindXDir", this(), 1, 5, this());

  if(!GetEffect("IntWindYDir", this()) && !GBackSolid(0, GetObjHeight()/2+2))
  {
    AddEffect("IntWindYDir", this(), 1, 10, this());
  }

//  if(GetComDir() == COMD_Stop && GBackSolid(0, GetObjHeight()/2+2))
//    SetComDir(COMD_Down);

  NoPilotCheck();
  CheckCommandAction();
  return(1);
}

private func FlightProcess()
{
  NoPilotCheck();
  CheckCommandAction();
  return(1);
}

private func CheckCommandAction()
{
  // Propeller an für Kommando
  if (GetCommand()) 
    if (GetAction()=="FloatIdle") 
      SetAction("FloatPropel");
  // Wenden nach rechts
  if (GetAction() == "FloatPropel")
    if (Inside(GetComDir(),COMD_UpRight,COMD_DownRight)) 
      if (GetDir()==DIR_Left)
        { SetAction("Turn"); SetDir(DIR_Right); return(true); }
  // Wenden nach links
  if (GetAction() == "FloatPropel")
    if (Inside(GetComDir(),COMD_DownLeft,COMD_UpLeft)) 
      if (GetDir()==DIR_Right)
        { SetAction("Turn"); SetDir(DIR_Left); return(true); }
  // Anhalten wenn Ziel erreicht
  if(!GetCommand() && !GetComDir())
    SetDirection(COMD_Stop);
  return(0);  
}   

private func NoPilotCheck()
{
  if (!FindObject(0,0,0,0,0,0,"Push",this())) 
    return(FloatIdleDown());
  return(0);
}

private func FloatIdleDown()
{
  if (GetAction() != "FloatIdle") {
    SetAction("FloatIdle");
  }
  SetComDir(COMD_Down);
  ClearCommand();
  return(1);
}

/* Trefferkontrolle */

protected func RejectCollect(id idObject, object pObj)
{
  // Pfeile vertragen sich nicht mit einem Luftschiff
  if (DefinitionCall(idObject,"IsArrow") )
   // Nur, wenn der Pfeil sich auch bewegt
   if(GetXDir(pObj) || GetYDir(pObj))
  {
    // bei brennendem Pfeil anzünden
    if(OnFire(pObj)) Incinerate();
    // ansonsten abstürzen
    else SetAction("DropOff");
    // in jedem Fall im Pfeil Hit auslösen (Sprengpfeil explodiert zB)
    ProtectedCall(pObj, "Hit");
    return(1);
  }
  return(1);
}

/* Angriff durch Vögel */

public func Poke()
{
  Sound("Rip");
  SetAction("DropOff");
  return(1);
}
  
/* Forschung */

public func GetResearchBase() { return(BALN); }
