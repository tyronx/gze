/*-- Vogel --*/

#strict 2
#include BIRD

public func IsPossessible() { return(0); }

public func SearchRange() { return(300); }

local pTarget;

/* TimerCall mit KI-Steuerung */

protected func Activity()
{
  // Geräusche machen
  if (!Random(25)) Sound("Raven*");

  // Gegner suchen
  if(!pTarget && !GetCommand())
    pTarget = FindObject(0, -SearchRange(), -SearchRange(), SearchRange()*2, SearchRange()*2, OCF_CrewMember, 0, 0, NoContainer());
  
  // Beute am Maul beißen
  if (Attack()) return;
  
  // Angreifen
  if(pTarget)
  {
    // Wenn Gegner im Haus, tot oder zu weit weg abbrechen	  
    if(Contained(pTarget))
	{
	  SetCommand(this, "None");
  	  return(pTarget=0);	 
	}
    if(!GetAlive(pTarget))
	{
	  SetCommand(this, "None");
	  return(pTarget=0);
	}
    if(ObjectDistance(pTarget)>SearchRange())
	{
	  SetCommand(this, "None");
	  return(pTarget=0);
	}
    // sonst zum Gegner fliegen
    var iFromSide = +1; if (GetX(pTarget)>GetX()) iFromSide = -1;
    SetCommand(this, "MoveTo", 0, GetX(pTarget) + 13 * iFromSide, GetY(pTarget));
    if(iFromSide>0 && GetDir()) TurnLeft();
    if(iFromSide<0 && !GetDir()) TurnRight();
    return;
  }
  
  // Nichts machen
  if (Random(2) || GetAction() != "Fly") return(1);

  // Nach oben oder unten fliegen
  SetComDir(COMD_Up);
  if (Random(2)) SetComDir(COMD_Down);

  // Nicht zu hoch fliegen
  if(GetY()<0) SetComDir(COMD_Down);

  // Nichts machen
  if (!Random(4)) return(1);

  // Umdrehen
  if (Random(2)) return(TurnRight());
  return(TurnLeft());
}

public func Attack(object pAttack)
{
  // Während des Fliegens kann alle 2 Sekunden einmal gebissen werden
  if (GetEffect("IntAttackDelay", this) || GetAction() != "Fly") return;

  // Beute suchen
  pAttack = FindObject(0, -14 + 28 * GetDir(), 0, 0, 0, OCF_Prey, 0, 0, NoContainer());
  
  // Einen Ballon oder ein Luftschiff suchen
  if (!pAttack) pAttack = FindObject(BALN, -20,0,40,30, 0, 0,0, NoContainer());
  if (!pAttack) pAttack = FindObject(BLMP, -30,0,60,40, 0, 0,0, NoContainer());
  if (!pAttack) return;

  if(GetAlive(pAttack))
  {
    // Schaden machen und schleudern	  
    Punch(pAttack);
    if(GetX(pAttack) < GetX(this))
      Fling(pAttack, -1, -1);
    else
      Fling(pAttack, 1, -1);
  }
  
  // Reinbeißen!
  pAttack->~Poke();
  SetAction("Attack");
  Sound("Raven*");
  AddEffect("IntAttackDelay", this, 1, 70);
  return(1);
}

protected func Death()
{
  ChangeDef(DBBR);
  return(1);
}
