/*-- Speer --*/

#strict 2

/* Status */
public func IsSpear() 
{ 
  return(1); 
}

public func CarryLimit()
{
  // Nur eines tragbar
  return(1);
}

public func Entrance()
{
  // Damit es die richtige Austrittsrotation annimmt
  SetRDir();
}

/* Werfen */
public func ControlThrow()
{
  // Träger ermitteln
  var pClonk = Contained();
  // Der Clonk kann dieses Objekt nicht verwenden
  if (!pClonk->~CanUse(GetID())) return(0);
  // Träger bereit zum Werfen?
  if (GetAction(pClonk) != "Walk" && GetAction(pClonk) != "Jump" && GetAction(pClonk) != "Ride" && GetAction(pClonk) != "RideStill") return;
  // Beim Träger Wurfaktion setzen (reitend)
  if (Contained()->~IsRiding())
    return(ObjectSetAction(Contained(), "RideThrowSpear"));
  // Beim Träger Wurfaktion setzen (normal)
  if (!ObjectSetAction(Contained(), "ThrowSpear")) return(0);
  // Fertig
  return(1);
}

/* Kann geschmiedet werden */
public func IsAnvilProduct() { return(1); }

