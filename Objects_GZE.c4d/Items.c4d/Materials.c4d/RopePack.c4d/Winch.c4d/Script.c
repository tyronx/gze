/*-- Seilwinde --*/

#strict 2

#include CU5P

local iPull, iPhase;

protected func Initialize()
{
  SetAction("Winding");
  iPull = 1;
  return 1;
}

func ControlUp()
{
  [$TxtCoil$|Image=CV5P:2]
  iPull = -5;
  return 1;
}

func ControlDownSingle()
{
  [$TxtUncoil$|Image=CV5P:0]
  iPull = 1;
  return 1;
}

func ControlLeft()
{
  [$TxtHold$|Image=CV5P:1]
  iPull = 0;
  return 1;
}

func ControlRight()
{
  [$TxtHold$|Image=CV5P:1]
  iPull = 0;
  return 1;
}

func RopeAskChangeLength(iLength, pBy)
{
  var iAngle;
  if(GetActionTarget(0, pBy) == this)
    iAngle = Angle(
      LocalN("aPointsX", pBy)[1],
      LocalN("aPointsY", pBy)[2],
      GetX() + GetVertex(0, 0),
      GetY() + GetVertex(0, 1));
  if(GetActionTarget(1, pBy) == this)
    iAngle = Angle(
      LocalN("aPointsX", pBy)[GetLength(LocalN("aPointsX", pBy)) - 2],
      LocalN("aPointsY", pBy)[GetLength(LocalN("aPointsY", pBy)) - 2],
      GetX() + GetVertex(0, 0),
      GetY() + GetVertex(0, 1));
  iAngle += 90;
  SetVertexXY(0, Sin(iAngle, 12), -Cos(iAngle, 12) - 2);

  iPhase = ((iPhase + 25) % 25);
  SetPhase(iPhase / 5);

  var i = 0;
  if(iPull <= 0 && iLength >= 25)
    i = 1;
  if(iPull < 0 && iLength <= 20 && EnergyCheck(10))
  {
    i -= 1;
    DoEnergy(-1);
  }
  if(iPull < 0 && iLength <= 10 && EnergyCheck(10))
    i -= 1;
  if(iPull > 0 && iLength >= 0 && ObjectCall(pBy, "GetLength") < 2000)
    i = 3;
  iPhase += i;

  return i;
}

func GetVertexToConnect() { return 1; }
