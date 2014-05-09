/*-- Holzfa� --*/

#strict 2
#include BARL

protected func EmptyBarrelID() { return(WDBR); }

/* TimerCall */

private func FillCheck()
{
  // Nicht wenn halb abgebrannt
  if (GetCon() < 100) return;
  // Material an aktueller Position �berpr�fen
  var iMaterial;
  if ( ((iMaterial = GetMaterial()) == -1) || (iMaterial == iBackMaterial) ) return(0);
  // Fass f�r dieses Material suchen
  var idBarrel;
  if (!(idBarrel = GetWoodBarrelType(iMaterial))) return(iBackMaterial = iMaterial);
  // Fass f�llen wenn entweder leer oder mit dem gleichen Material aufgef�llt werden kann
  if(BarrelMaterial() != -1 && BarrelMaterial() != iMaterial) return(0);
  // Wenn nicht bereits voll
  if(iFillLevel >= BarrelMaxFill()) return(0);
  // Fass f�llen
  iFillLevel += ExtractMaterialAmount(0, 0, iMaterial, BarrelMaxFill() - iFillLevel);
  // Zum vollen Fass werden, wenn wir es nicht bereits sind
  if(BarrelMaterial() == -1) ChangeDef(idBarrel);
  return(1);
}

/* Aufschlag */ 
  
protected func Hit()
{
  Sound("Woodbarrel");
  BarrelEject();
  return(1);
}

/* Entz�ndung */

protected func Incineration()
{
  // Verdunkeln, damit man besser sieht dass es unbenutzbar ist
  SetClrModulation(0x5f5f4f);
  return _inherited(...);
}
