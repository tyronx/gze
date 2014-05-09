/*-- Schild --*/

#strict 2

/* Status */
public func IsShield()
{
  return(1);
}

// Nur beim Schild selbst Grafikname angeben,
// damit alte �berladungen noch das alte Prinzip verwenden 
public func GetShieldGraphics() { if(GetID()==SHIE) return("Shield"); }

/* Anlegen */
public func Activate(pKnight)
{      
  [$CtrlActivateDesc$]             
  // Der Clonk kann dieses Objekt nicht verwenden
  if (!pKnight->~CanUse(GetID())) return(0);
  // Ritter tr�gt schon ein Schild
  if (pKnight->~HasShield()) return(0);
  // Versuchen als Grafik anzulegen
  if(GetShieldGraphics())
   if(pKnight->~WearShield(this(), GetShieldGraphics()))
    return(1);
  // sonst altes Anlegen
  Sound("Connect");
  SetOwner(GetOwner(pKnight));
  ChangeDef(SHIA);
  SetAction("Shield",pKnight);
  return(1);
}

/* Aufschlag */  
protected func Hit()
{
  Sound("MetalHit*");
}

/* Kann geschmiedet werden */
protected func IsAnvilProduct() { return(1); }
