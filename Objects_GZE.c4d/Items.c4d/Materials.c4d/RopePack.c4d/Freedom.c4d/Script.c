/*-- Freiheit --*/

#strict 2
#include MELE

local iFreeCount;

func Initialize()
{
  var pGoal = FindObject2(Find_ID(GetID()), Find_Exclude());
  if(pGoal)
  {
    LocalN("iFreeCount", pGoal)++;
    return RemoveObject();
  }
  iFreeCount++;
  ScheduleCall(0, "CheckFree", 30);
  return _inherited();
}

private func CheckFree()
{
  for(var i = 0; i < GetPlayerCount(); i++)
  {
    var iPlr = GetPlayerByIndex(i);

    // Falls zu wenige freie Clonks, Spieler eliminieren
    if(GetFreeClonks(iPlr) <= iFreeCount)
    {
      Log("$MsgPlayerElimnated$", GetPlayerName(iPlr));
      EliminatePlayer(iPlr);
      return 0;
    }
  }
  return ScheduleCall(0, "CheckFree", 30);
}

global func GetFreeClonks(iPlr)
{
  var iFree;
  for(var i = 0; i < GetCrewCount(iPlr); i++)
    if(!GetEffect("IntTied", GetCrew(iPlr, i))) ++iFree;
  return iFree;
}
