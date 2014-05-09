/*-- Kochtopf --*/

#strict 2

local pFire;

/* Initialisierung */

protected func Initialize() { pFire = CreateObject(CPFR,5,+25); }

/* TimerCall */

private func CheckContents()
{
  if(GetAction(FindObject(CPFR,-50,0,100,100)) == "Burn")
    if(Contents()) 
      if(IsHot())
        for(var i=0;i<ContentsCount();++i)
          if(Contents(i)->~Cook())
            Sound("Cook");
}

/* Aufnahme */

protected func Collection() { Sound("Clonk"); }
  
/* Aufschlag */  
  
protected func Hit() { Sound("ClonkHit*"); }

/* Campfeuer brennt */
protected func IsHot() 
{ 
  if(!pFire) 
    return; 
  return(GetAction(pFire) == "Burn"); 
}

// kann durch das Amulett des Gelehrten erforscht werden
public func IsJungleResearch() { return(1); }

// ohne H�tte keinen Kochtopp
public func GetResearchBase() { return(HUT1); }
