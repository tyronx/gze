/*-- Kleiner Hochofen --*/

#strict 2
#include BAS2

/* Steuerung */

public func ContextProduction(pCaller)
{
  [$ProduceMetal$|Image=METL]
  return(ProductionOrder(METL,pCaller));
}
  
public func ControlDigDouble(pCaller)
{
  [$ProduceMetal$|Image=METL]
  return(ProductionOrder(METL,pCaller));
}


/* Produktion */

public func IsProducerOf(clonk, def) 
{
  return (def == METL && GetCon() >= 100);
}

public func HowToProduce (clonk, def)
{
  AddCommand(clonk, "Wait", 0, 0, 0, 0, 0, 190);
  ProductionOrder(def, clonk);
}

public func ProductionOrder(idProduct, pWorker)
{
  // Material beschaffen
  if (!HasFuel())
    ProductionAcquireMaterial(COAL,1,pWorker);
  else
    ProductionAcquireMaterial(ORE1,1,pWorker);
  // Brennvorgang beginnt selbstt�tig
  return(1);
}

private func ProductionAcquireMaterial(idMat, iAmount, pWorker)
{
  // Ben�tigte Menge ermitteln
  var iNeededAmount = Max(iAmount - ContentsCount(idMat));
  // Der Arbeiter soll das Material beschaffen
  AddCommand(pWorker,"Call",this,0,0,0, 0, Format("CmdAcquire%i", idMat), 0, 3);
  while (iNeededAmount--)
  {
    AddCommand(pWorker,"Put",this,0,0,0, 0, idMat);
    AddCommand(pWorker,"Acquire",0,0,0,this, 0, idMat);
  }
  return(1);
}

protected func CmdAcquireORE1(pWorker) { return(1); } // Erz erfolgreich beschafft: Fertig
protected func CmdAcquireCOAL(pWorker) { return(ProductionAcquireMaterial(ORE1,1,pWorker)); } // Kohle erfolgreich beschafft: Erz beschaffen
protected func CmdAcquireOBRL(pWorker) { return(ProductionAcquireMaterial(ORE1,1,pWorker)); } // �lfass erfolgreich beschafft: Erz beschaffen

protected func CmdAcquireORE1Failed(pWorker)
{
  // Fehler bei der Materialbeschaffung: Meldung
  return(Message("$MsgOreNotAvailable$", pWorker));
}

protected func CmdAcquireCOALFailed(pWorker)
{
  // Keine Kohle vorhanden: Holz suchen
  return(ProductionAcquireMaterial(OBRL,1,pWorker));
}

protected func CmdAcquireOBRLFailed(pWorker)
{
  // Fehler bei der Materialbeschaffung: Meldung
  return(Message("$MsgFuelNotAvailable$", pWorker));
}

/* Aufnahme */

protected func Collection() { return(Sound("Clonk")); }
  
/* Brennvorgang */

protected func Burning()
{
  // Effekte
  Smoke(0, 10, 6);
  // Noch nicht fertig
  if (GetActTime() < 150) return(1);
  // Aktion beenden
  SetAction("Idle");
  // Effekt
  CastParticles("PxSpark",6,10,0,10,15,30,RGB(255,223,127),RGB(255,223,127));
  // Erz in Metall umwandeln
  var pOre;
  if (pOre = FindContents(ORE1))
  {
    RemoveObject(pOre);
    Exit(CreateContents(METL), -19,10,0, -1);
  }
  Sound("Pshshsh");
  // Fertig
  // Pr�fen, ob der Ofen weiterlaufen kann
  return(ContentsCheck());
}

/* Brennmaterial suchen */

protected func Collection2()
{
  // Neuer Inhalt: Pr�fen (aber nicht sofort wieder auswerfen; sieht bl�d aus)
  if (ActIdle()) SetAction("Wait");
  return(1);
}

protected func ContentsCheck()
{
  // Unpassende Gegenst�nde auswerfen
  var i, pContents, idContents;
  while (pContents = Contents(i++))
  {
    idContents = GetID(pContents);
    if (idContents != COAL && idContents != OBRL && idContents != ORE1)
      Exit(pContents, -20,13,0, -1);
  }
  // Ofen l�uft schon?
  if (!ActIdle()) return(1);
  // Nur starten, wenn Erz drin ist
  if (!FindContents(ORE1)) return(1);
  // Brennmaterial suchen
  var pBurnMat;
  
  if (pBurnMat = FindContents(COAL))
    RemoveObject(pBurnMat);
  else if (pBurnMat = FindContents(OBRL))
    ChangeDef(BARL,pBurnMat);
  else return(1);
  // Ofen anschmei�en
  SetAction("Burning");
  return(1);
}

public func HasFuel()
{
  if (FindContents(COAL)) return(1);
  if (FindContents(OBRL)) return(1);
  return;
}


/* Status */

protected func CalcValue() // Geringer Verkaufswert
{
  if (Contained()) return(25);
  return(50);
}
