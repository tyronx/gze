/*-- Ritter --*/

#strict 2

#include CLNK

/* Lokale Variablen */

local iOrder, pOrdrTarget;
local ordrData1, ordrData2;
local iLastAimAngle;
local fArmored, idArmorType;
local idShield;

// Vom K�nig ueberladbar, gibt Anzahl an Standard-Portraits an
public func StdPortraitCount() { return(3); }

/* Bei dem Beitritt zum Spieler Portraits pr�fen */

public func Recruitment(int player)
{
  // Zur Netzwerk-Sicherheit
  var iRandomPortrait = Random(StdPortraitCount());
  // Mal schauen, was fuer ein Portrait wir haben
  var portrait = GetPortrait(this(), 0, 1);
  var portrait_id = GetPortrait(this(), 1, 1);
  // Ist es ein eigenes Portrait? Dann lassen wir es.
  if(portrait == "custom") return(_inherited() );
  // Ist eine ID angegeben?
  if(portrait_id && portrait_id != NONE)
  {
    // Kein Ritterportrait?
    if(portrait_id != GetID() )
    {
      SetPortrait(Format("%d", 1 + iRandomPortrait), this(), GetID(), true, false);
      return(_inherited() );
    }
  }
  // Es handelt sich um ein Ritterportrait. Aber ein richtiges?
  if(!WildcardMatch(portrait, "?") )
  {
    // Nein, das war ein Ruestungsportrait oder sowas. Wir wollen aber lieber ein normales.
    SetPortrait(Format("%d", 1 + iRandomPortrait), this(), GetID(), true, false);
  }
  return(_inherited(player) );
}

public func ExecuteOrder() {
}

/*--------------------------------------------- Status -------------------------------------------------*/

//! Welche Objekte kann dieser Clonk einsetzen?
public func CanUse(def)
{
  // Schwert
  if (def == SWOR) return(1);
  // Axt
  if (def == AXE1) return(1);  
  // Bogen 
  if (def == BOW1) return(1);   
  // Schild 
  if (def == SHIE) return(1);   
  // Speer 
  if (def == SPER) return(1);   
  // Lanze 
  if (def == LANC) return(1);   
  // R�stung 
  if (def == ARMR) return(1);  
  // Schwarze R�stung 
  if (def == BRMR) return(1);  
  // Unbekannt 
  return(0); 
} 
 
//! Bevorzugte Waffentypen f�r selbst�ndige Bewaffnung (zur Zeit werden genau zwei Waffentypen erwartet). 
public func GetFightWeaponType(index) 
{ 
  if (index == 0) return(SWOR); 
  if (index == 1) return(AXE1); 
  return(0); 
} 
 
//! Dieser Clonk ist ein Ritter
public func IsKnight()  
{  
  return(1);  
} 
 
//! Reitet der Clonk gerade? 
private func IsRiding() 
{
  return (WildcardMatch(GetAction(), "Ride*"));
} 
 
//! Zielt der Clonk gerade mit dem Bogen? 
private func IsAiming() 
{ 
  return (GetAction() == "AimBow" || GetAction() == "RideAimBow"); 
} 
 
//! L�dt der Clonk gerade den Bogen nach? 
private func IsLoading() 
{ 
  return (GetAction() == "LoadBow" || GetAction() == "RideLoadBow"); 
} 
 
//! Legt der Clonk gerade eine R�stung an? 
private func IsPuttingOnArmor() 
{ 
  return (GetAction() == "PutOnArmor"); 
} 
 
//! Der Clonk gilt als bewaffnet, wenn er mindestens ein Schild oder eine R�stung nebst einer Handwaffe tr�gt.
private func IsFullyArmed() 
{ 
  return (HasFightWeapon() && (HasShield() || HasArmor())); 
} 
 
//! Nicht voll bewaffnet. 
public func IsNotFullyArmed()  
{  
  return (!IsFullyArmed());  
} 
 
//! Tr�gt der Clonk einen Bogen und hat Pfeile dabei? 
private func IsBowEquipped() 
{ 
  // Bogen dabei? 
  if (!FindContents(BOW1)) return(0); 
  // Pfeile dabei? 
  if (!GetArrowCount()) return(0); 
  // Bogen und Pfeile dabei 
  return(1); 
} 
 
//! Liefert den Schutzfaktor. 
public func IsShielded(fArrow) 
{ 
  var iShielded = 0, iShieldFactor; 
  // Schild 
  var pShield; 
  if (pShield = HasShield()) 
    // Benutzerdefiniertes Schild 
    if (iShieldFactor = pShield->~ShieldFactor(fArrow)) 
      iShielded += iShieldFactor; 
    // Normales Schild 
    else 
      // Das Standardschild hat einen Bonus gegen Pfeile 
      if (fArrow) 
        iShielded += 20; 
      else 
        iShielded += 10; 
  // R�stung                    
  if (HasArmor()) 
    iShielded += DefinitionCall(idArmorType, "ShieldFactor"); 
  // Wert zur�ckliefern 
  return(iShielded); 
} 
 
//! Pr�ft ob pObj (oder dieser Clonk) ein Schild tr�gt. 
public func HasShield(pObj)  
{  
  if (!pObj) pObj = this();
  if(LocalN("idShield", pObj)) return(LocalN("idShield", pObj));
  return (FindObject(0,0,0,0,0,0, "Shield", pObj));  
}

//! Pr�ft ob pObj (oder dieser Clonk) eine R�stung tr�gt. 
public func HasArmor(pObj)  
{ 
  if (pObj) return(pObj->~HasArmor());  
  return(fArmored);  
} 
 
//! Pr�ft ob der Clonk eine Handwaffe tr�gt, die er verwenden kann. 
public func HasFightWeapon(pClonk) 
{  
  // Anderer Clonk 
  if (pClonk)  
    return (pClonk->~HasFightWeapon()); 
  // Dieser Clonk 
  var pObj; 
  for (var i = 0; pObj = Contents(i); i++) 
    if (pObj->~IsFightWeapon() && CanUse(GetID(pObj))) 
      return(1); 
  // Keine Waffe 
  return(0); 
} 
 
//! Liefert die erste gefundene Handwaffe, die der Clonk verwenden kann. 
public func GetFightWeapon(pClonk) 
{  
  // Anderer Clonk 
  if (pClonk)  
    return (pClonk->~GetFightWeapon()); 
  // Dieser Clonk 
  var pObj; 
  for (var i = 0; pObj = Contents(i); i++) 
    if (pObj->~IsFightWeapon() && CanUse(GetID(pObj))) 
      return(pObj); 
  // Keine Waffe 
  return(0); 
} 
 
//! Kann der Clonk aus der aktuellen Aktion den Bogen oder eine Wurfwaffe einsetzen? 
public func ReadyToFire() 
{ 
  // Nur beim Laufen - und neu auch beim Reiten... 
  if (GetAction() == "Walk" || GetAction() == "Ride" || GetAction() == "RideStill") return(1); 
  return(0); 
} 
 
//! Der Ritter braucht einen Sattel zum Reiten. 
public func NeedSaddle()  
{  
  return(1);  
} 
  
/*------------------------------------------- Steuerung ------------------------------------------------*/   
 
/* An Inhalt weitergeben, sonst internen Befehl ausf�hren. */ 
/* Direkte Steuerung l�scht Befehle. */ 
 
/* Steuerung Werfen */ 
protected func ControlThrow(object pByObject)
{ 
  // Bei vorherigem Doppel-Stop nur Ablegen   
  if (GetPlrDownDouble(GetOwner())) return(0);  
  // Steuerung an Inhalt/Fahrzeug weitergeben 
  if (Control2Contents("ControlThrow")) return(1); 
  if (Control2Vehicle("ControlThrow")) return(1); 
  // Sonst internen Throw-Befehl ausf�hren 
  return(_inherited(pByObject)); 
} 
 
protected func ControlUp(object pByObject)
{ 
  
  if ( Control2Vehicle("ControlUp") ) return(1); 
  if ( Control2Contents("ControlUp") ) return(1); 
  return(_inherited(pByObject)); 
} 
protected func ControlUpDouble(object pByObject)
{ 
  
  if ( Control2Vehicle("ControlUpDouble") ) return(1); 
  if ( Control2Contents("ControlUpDouble") ) return(1); 
  return(_inherited(pByObject)); 
} 
protected func ControlDown(object pByObject)
{ 
  
  if(IsFighting() ) 
  { 
    StopFighting(); 
    return(1); 
  }
  if ( Control2Vehicle("ControlDown") ) return(1); 
  if ( Control2Contents("ControlDown") ) return(1); 
  return(_inherited(pByObject)); 
} 
protected func ControlDownSingle(object pByObject) 
{ 
  
  if ( Control2Vehicle("ControlDownSingle") ) return(1); 
  if ( Control2Contents("ControlDownSingle") ) return(1); 
  return(_inherited(pByObject)); 
} 
protected func ControlDownDouble(object pByObject)
{ 
  
  if ( Control2Vehicle("ControlDownDouble") ) return(1); 
  if ( Control2Contents("ControlDownDouble") ) return(1); 
  return(_inherited(pByObject)); 
} 
protected func ControlLeft(object pByObject) 
{ 
  
  if ( Control2Vehicle("ControlLeft") ) return(1); 
  if ( Control2Contents("ControlLeft") ) return(1); 
  return(_inherited(pByObject)); 
} 
protected func ControlRight(object pByObject) 
{ 
  
  if ( Control2Vehicle("ControlRight") ) return(1); 
  if ( Control2Contents("ControlRight") ) return(1); 
  return(_inherited(pByObject)); 
} 
protected func ControlLeftDouble(object pByObject) 
{ 
  
  if ( Control2Vehicle("ControlLeftDouble") ) return(1); 
  if ( Control2Contents("ControlLeftDouble") ) return(1); 
  return(_inherited(pByObject)); 
} 
protected func ControlRightDouble(object pByObject)
{ 
  
  if ( Control2Vehicle("ControlRightDouble") ) return(1); 
  if ( Control2Contents("ControlRightDouble") ) return(1); 
  return(_inherited(pByObject)); 
} 
protected func ControlDigDouble(object pByObject) 
{ 
  
  if ( Control2Vehicle("ControlDigDouble") ) return(1); 
  if ( Control2Contents("ControlDigDouble") ) return(1); 
  return(_inherited(pByObject)); 
} 
protected func ControlDig(object pByObject) 
{ 
  
  if ( Control2Vehicle("ControlDig") ) return(1); 
  if ( Control2Contents("ControlDig") ) return(1); 
  return(_inherited(pByObject)); 
}

protected func ControlUpdate(object pByObject, int comdir, bool dig, bool throw)
{
  // Clear orders (if there is a comdir set)
  
  if( Control2Vehicle("ControlUpdate", comdir, dig, throw)) return(1);
  if( Control2Contents("ControlUpdate", comdir, dig, throw)) return(1);
  return(_inherited(pByObject, comdir, dig, throw));
}
 
// Kontextmen�

public func IsBuckled()
{
  return(FindAttachedItems() || HasArmor() || HasShield() );
}

public func ContextUnbuckle()
{
  [$CtxUnbuckleDesc$|Image=KMSU]//|Condition=IsBuckled]
  ShowUnbuckleMenu();
  return(1);
}

private func ShowUnbuckleMenu()
{
  var obj;
  CreateMenu(KMSU, 0, 0, 0, "$CtxUnbuckleDesc$", 0, 1);
  while(obj = FindAttachedItems(obj) )
    AddMenuItem(Format("$CtxUnbuckleItem$", GetName(0, obj->UnbuckledID()) ), "Unbuckle", GetID(obj), 0, 0, obj );
  if(idShield) AddMenuItem(Format("$CtxUnbuckleItem$", GetName(0, idShield)), "RemoveShield", idShield);
  if(HasArmor()) AddMenuItem(Format("$CtxUnbuckleItem$", GetName(0, idArmorType)), "RemoveArmor",idArmorType);
}

private func Unbuckle(id dummy, object obj)
{
  obj->~Unbuckle();
}
 
// Damit Aufrufe keinen Fehler verursachen, wenn bestimmte Pakete nicht aktiviert sind
global func MMStartPraying(par1,par2,par3){return(_inherited(par1,par2,par3));}
global func MMStopPraying(par1,par2,par3){return(_inherited(par1,par2,par3));}
global func MMGetChurch(par1,par2,par3){return(_inherited(par1,par2,par3));}

private func PrayingAllowed()
{
 // Zum ueberpruefen, ob entsprechendes vorhanden ist
 if(!GetName(0,CHCS) || !GetName(0,KRME))return(false);
 if(!GetActMapVal("Length","Pray",GetID()))return(false);
 return(!ObjectCount(NPRY) && !Contained() && MMGetChurch(GetOwner()));
}

public func ContextPray()
{
  [$CtxPrayDesc$|Image=CHCS|Condition=PrayingAllowed]
  if(GetAction() == "Walk")
  SetAction("Pray");
  else if(GetAction() == "Pray")
  SetAction("KneelUp");
  return(1);
}

protected func StartPraying()
{
  // Wenn das Objektpaket nicht geladen ist, einfach nicht einmischen
  if(!GetName(0,CHCS) || !GetName(0,KRME))return(false);
  
  var iAmount = 5;
  if(FindObject2(Find_ID(SNHP), Find_AtPoint())) iAmount = 10;
  else if(FindObject2(Find_ID(ALTR), Find_AtPoint())) iAmount = 7;
  if(!MMStartPraying(iAmount)) SetAction("Walk");
}

protected func StopPraying()
{
  // Wenn das Objektpaket nicht geladen ist, einfach nicht einmischen
  if(!GetName(0,CHCS) || !GetName(0,KRME))return(false);
  
  if(GetAction()=="Pray") return;
  MMStopPraying();
}
 
/*------------------------------------------ Kommandosteuerung ------------------------------------------------*/   
 
protected func ControlCommand(szCommand, pTarget, iTx, iTy, pTarget2, Data) 
{ 
  // Context -> Keine Sonderbehandlung 
  if(szCommand == "Context") return(0); 
  
  // Kommando MoveTo an Pferd weiterleiten. 
  if (IsRiding()) 
    { 
    if (szCommand == "MoveTo") 
      return(ObjectCall(GetActionTarget(), "ControlCommand", szCommand, pTarget, iTx, iTy)); 
    // Anderes Kommando beim Reiten: absteigen 
    SetComDir(COMD_Stop, GetActionTarget()); 
    ObjectCall(GetActionTarget(), "ControlDownDouble", this); 
    } 
  // Kommando MoveTo in Bogenschie�en umwandeln. 
  if (szCommand == "MoveTo") 
    { 
    if (IsAiming()) 
      { FireBowAt(iTx, iTy, true); return(1); } 
    // Der Spieler war etwas schnell... 
    if (IsLoading()) 
      return(1); 
    } 
  
  // Sonst nicht abfangen 
  return(_inherited(szCommand, pTarget, iTx, iTy, pTarget2, Data) ); 
} 
 
/* Spezialtaste: Inventarwechsel */ 
protected func ControlSpecial() 
{ 
  [$CtrlInventoryDesc$|Image=CXIV]
  // Zielaktion abbrechen 
  StopAiming();  
  // Inventory verschieben 
  return (ShiftContents(0,0,0,1)); 
} 
 
// Inventarauswahl 
protected func ControlContents() 
{ 
  // Zielaktion abbrechen 
  StopAiming();  
  // Standardaktivit�t 
  return(0); 
} 
 
/* Leitet die Steuerung an das erste Inhaltsobjekt weiter.  
   R�ckgabewert 1 wenn erfolgreich */ 
private func Control2Contents(szControl)
{ 
  // Getragenes Objekt hat spezielle Steuerungsauswertung 
  if (ObjectCall(Contents(),szControl, this(), ...) ) 
    return(1); 
  // Keine Inhaltssteuerung 
  return(0); 
} 
     
/* Leitet die Steuerung an ein ggf. gesteuertes Fahrzeug weiter. 
   R�ckgabewert 1 wenn erfolgreich */ 
 
private func Control2Vehicle(szControl) 
{ 
  // Reiten und Werfen - an dieser Stelle doppelt gemoppelt, weil schon im normalen Clonk-Script.
  /*if (IsRiding()) 
    if (szControl == "ControlThrow") 
      if (Contents()) 
      { 
        SetAction("RideThrow"); // Bemerke: ActionTarget wird beibehalten 
        return(); 
      }*/
  // Reiten: Steuerung an Pferd weiterleiten 
  if (IsRiding()) 
    if (ObjectCall(GetActionTarget(),szControl, this(), ...)) 
      return(1); 
 
  // Keine Fahrzeugsteuerung 
  return(0); 
} 
 
/*-------------------------------------------- Pfeile / Pakete -----------------------------------------*/ 
 
/* Zielaktion abbrechen */ 
public func StopAiming() 
{ 
  // Zielt gar nicht: nix machen 
  if (!IsAiming()) return(0); 
  // Stop 
  SetComDir(COMD_Stop); 
  // Basisaktion setzen 
  if (IsRiding()) 
    SetAction("Ride"); 
  else 
    SetAction("Walk"); 
  // Sound 
  Sound("Grab"); 
  return(1); 
} 
 
/* �berpr�ft ob der �bergebene Typ ein Pfeil ist  */ 
private func CheckArrow(idDef) { return(idDef->~IsArrow()); } 
 
/* �berpr�ft ob der �bergebene Typ ein Pfeilpaket ist */ 
private func CheckArrowPack(idDef) { return(idDef->~IsArrowPack()); } 
 
private func MaxContentsCount() { if (HasArmor() || IsPuttingOnArmor()) return(2); else return(3); } 
 
 
/* Aufnahme */
protected func Collection2(pObj) 
  { 
  // �bergeordnete Aufnahme 
  _inherited(pObj); 
  // Nicht mehr enthalten (oder gel�scht): Abbrechen 
  if (Contained(pObj) != this()) return(1); 
  // Bogen nicht mehr am ersten Inventarplatz?
  if (Contents() && IsAiming())
    if (!Contents()->~IsAiming() && GetID(pCont)!=BOW1)
      {
      // Erstmal versuchen, den Bogen wieder anzuw�hlen...
      var i,pCont;
      while (pCont=Contents(++i)) if (pCont->~IsAiming() || GetID(pCont)==BOW1) break;
      if (pCont)
        ShiftContents(0, false, GetID(pCont));
      else
        // Neuanwahl erfolglos (warum auch immer): Zielen abbrechen
        StopAiming();
      }
  return(1); 
  }
  
public func GetMaxSpecialCount(iIndex, fAmount)
{
  // Der Ritter kann zus�tzlich noch 30 Pfeile tragen
  if(iIndex == 0) { if(fAmount) return(30); return("IsArrow"); }
}
 
protected func GetObject2Drop(pCollectObj) 
  { 
  var iArrowPriority; 
  // Wenn ein Pfeil oder Pfeilpaket aufgenommen werden soll... 
  if (pCollectObj) if (ObjectCall(pCollectObj, "IsArrow") || ObjectCall(pCollectObj, "IsArrowPack")) 
    // Pfeilen und Pfeilpaketen eine niedrigere Priorit�t geben 
    iArrowPriority=-500; 
  // Das schlechteste Objekt wegwerfen 
  var pBestDropObj, iBestDropPrio=1000; 
  var pObj, iPrio; 
  var idCollectObj = GetID(pCollectObj); 
  for(var i = 0; pObj = Contents(i); i++) 
    if ((iPrio = GetContentsPriority(pObj, iArrowPriority, idCollectObj)) < iBestDropPrio) 
      { 
      iBestDropPrio = iPrio; 
      pBestDropObj = pObj; 
      } 
  // Dieses Objekt zur�ckgeben 
  return(pBestDropObj); 
  } 
 
private func GetContentsPriority(pContents, iArrowPriority, idCollectObj) 
  { 
  // Pfeil? Normalerweise hohe Priorit�t, wenn nicht gerade ein anderes Pfeilpaket eingesammelt werden soll 
  if (pContents->~IsArrow()) return(302+iArrowPriority); 
  // Pfeilpakete sind etwas geringer brauchbar, damit sie eher abgelegt werden (geht schneller...) 
  if (pContents->~IsArrowPack()) return(301+iArrowPriority); 
  // Objekt ist eine Handwaffe? 
  var idCnt = GetID(pContents); 
  if (pContents->~IsFightWeapon()) 
    { 
    // Es soll eine Handwaffe eingesammelt werden 
    if (DefinitionCall(idCollectObj, "IsFightWeapon")) 
      // Hat schon eine Handwaffe 
      if (HasFightWeapon()) 
        // Sehr niedrige Priorit�t 
        return(1); 
    // Ansonsten relativ hohe Priorit�t 
    return(220); 
    } 
  // Bogen? 
  if (idCnt==BOW1) 
    { 
    // Noch ein Bogen vorhanden? Muss nicht sein 
    if (idCollectObj==BOW1 || ContentsCount(BOW1) > 1) return(1); 
    // Ansonsten fast so wichtig wie die Handwaffe (f�r die KI...) 
    return(216); 
    } 
  // Speer? 
  if (idCnt==SPER) return(212); 
  // Der Rest kann kaum gebraucht werden; ablegen nach Wert 
  return(GetValue(pContents)); 
  } 
  
/*---------------------------------------------- Aktionen ------------------------------------------------*/ 
  
private func Throwing(pObj) 
{            
  // Ggf. zu werfendes Objekt suchen 
  if (!pObj) 
  { 
    pObj = Contents(0); 
    if (!Contents()) return(0); 
  } 
  // Wurfparameter berechnen 
  var iX, iY, iR, iXDir, iYDir, iRDir; 
  if(GetID(pObj) != SPER && !pObj->~IsSpear()) 
  { 
    // Standardwurf   
    iX = 0; if (!GetDir()) iX = -iX; 
    iY = -10; 
    iR = Random(360); 
    iXDir = GetPhysical("Throw") / 25000; if(!GetDir()) iXDir = -iXDir; 
    iYDir = -GetPhysical("Throw") / 25000; 
    iRDir = Random(40) - 20; 
  } 
  else 
  { 
    // Speerwurf 
    iX = 10; if (!GetDir()) iX = -iX; 
    iY = -6; 
    iR = 150; if(!GetDir()) iR = 180 - iR; 
    iXDir = 5; if(!GetDir()) iXDir = -iXDir; 
    iYDir = -2; 
    iRDir = 4; if(!GetDir()) iRDir = -iRDir; 
  } 
  // Reitet? Eigengeschwindigkeit addieren 
  if (GetActionTarget()) if (IsRiding()) 
  {
    iXDir += GetXDir(GetActionTarget()) / 10;
    // Steigungen beim Gehen werden leider nicht in die Y-Dir eingerechnet. Bei Steigungen
    // kann es daher vorkommen, dass der Wurf den Reiter vom Pferd haut. Was dagegen tun..?
    iYDir += GetYDir(GetActionTarget()) / 10; 
  } 
  // Werfen! 
  Exit(pObj, iX, iY, iR, 0, 0, iRDir); 
  pObj->SetXDir(GetXDir()+iXDir*10);
  pObj->SetYDir(GetYDir()+iYDir*10);
  return(1);   
}

func ExitItem() {
	return (Throwing(Contents(0)));
}
 
/* Speer werfen */ 
private func ExitSpear() 
{ 
  return(Throwing(FindSpearContents())); 
} 
 
/* Speer im Inventar finden */ 
private func FindSpearContents() 
  { 
  var pObj,i; 
  // SPER suchen 
  if (pObj = FindContents(SPER)) return (pObj); 
  // Objekte mit IsSpear() suchen 
  while (pObj=Contents(i++)) if (pObj->~IsSpear()) return (pObj); 
  // Nichts gefunden 
  return;
  } 
 
// Pr�ft, ob der Clonk gerade kaempft. 
private func IsFighting() 
{ 
  if (WildcardMatch(GetAction(),"*Fight") ) return(true); 
  if (WildcardMatch(GetAction(),"*Strike") ) return(true); 
  if (WildcardMatch(GetAction(),"*Punch*") ) return(true); 
  if (WildcardMatch(GetAction(),"*Thrust") ) return(true); 
  return(false); 
} 
 
// Erst 25 Frames nach dem ControlDown-Kommando 
private func StopFighting() 
{ 
  // ComDir auf NONE setzen, damit der Clonk nach dem Kampfabbruch nicht weiterlaeuft. 
  SetComDir(COMD_None ); 
  // StopFighting-Effekt anfuegen, der den Kampf nach 25 Frames abbricht, wenn nicht schon vorhanden. 
  if(!GetEffect("IntKNIGStopFighting",this,0,1) ) AddEffect("IntKNIGStopFighting", this, 1, 25, this ); 
} 
 
protected func FxIntKNIGStopFightingStop() 
{ 
  // Kaempfen wir noch? 
  if(IsFighting() ) 
  { 
    // Letzten Gegner fuer weitere 25 Frames speichern 
    AddEffect("IntKNIGLastEnemy", this, 1, 25, this, 0, GetActionTarget() ); 
    // Kampf abbrechen 
    SetAction("Walk"); 
  } 
} 
 
protected func FxIntKNIGLastEnemyStart(pTarget, iEffect, pEnemy) 
{ 
  EffectVar(0, pTarget, iEffect) = pEnemy; 
} 
 
protected func RejectFight(object pEnemy) 
{ 
  // Ist ein letzter Gegner gespeichert? 
  if(GetEffect("IntKNIGLastEnemy", this, 0, 1) ) 
    // Ist der letzte Gegner der, mit dem wir hier kaempfen wuerden? 
    if(pEnemy == EffectVar(0, this, GetEffect("IntKNIGLastEnemy", this) ) ) 
      // Ja, also abbrechen 
      return(1); 
  // Kampf erlauben 
  return(0); 
} 
 
/* W�hrend des K�mpfens */ 
private func Fighting(pEnemy) 
{ 
  // Kampf mit Handwaffe 
  var pWeapon; 
  if (pWeapon = GetFightWeapon()) 
  { 
    // Erstmal Kampfaktion setzen 
    if (GetAction() != pWeapon->~GetFightAction()) 
      return (SetAction(pWeapon->~GetFightAction(), pEnemy)); 
    // Je nach Waffengeschwindigkeit Schlag durchf�hren 
    if (!Random(pWeapon->~GetStrikeTime())) 
      return (SetAction(pWeapon->~GetStrikeAction())); 
    // Weiterk�mpfen 
    return(1); 
  } 
  // Keine Waffe: Gegner schlagen 
  if (!Random(4))  
    SetAction("Punch"); 
  // Weiterk�mpfen 
  return(1); 
} 

protected func Death(int iKiller, a,b,c,d,e)
{
  // Angelegtes Zeugs ablegen
  var obj;
  while(obj = FindAttachedItems(obj) ) ProtectedCall(obj, "AttachTargetLost");
  // Schild ablegen
  RemoveShield();
  // R�stung ablegen
  ArmorRemoved(); while (Contents()) Contents()->Exit();
  // Basisfunktion aufrufen
  return(_inherited(iKiller, a,b,c,d,e) );
}
 
/* Schlag mit Handwaffe */ 
private func Striking() 
{ 
  // Waffe bestimmen 
  var pWeapon; 
  if (!(pWeapon = GetFightWeapon())) return(0); 
  // Sound 
  pWeapon->~StrikeSound(); 
  // Kein Schaden 
  if (!Random(2)) return(1); 
  // Schaden ausrechnen 
  var pEnemy = GetActionTarget(); 
  var iDamage = pWeapon->~GetStrikeDamage(); 
  iDamage = iDamage * (100 - pEnemy->~IsShielded()) / 100; 
  // Schlagen! 
  Punch(pEnemy, iDamage); 
  return(1); 
} 
 
protected func QueryCatchBlow(pObj) 
{ 
  // Schutzfaktor: l�sst Objekte zur�ckprallen 
  var iPower = BoundBy(GetMass(pObj), 0, 50) * Distance(GetXDir(pObj), GetYDir(pObj)) * Distance(GetXDir(pObj), GetYDir(pObj));   if(Random(50000) + iPower / 3 < IsShielded() * 1000) 
  {           
    // Projektil abprallen lassen 
    if(GetXDir(pObj) < 0) if(GetX(pObj) < GetX() + 6) SetPosition(GetX() + 9, GetY(pObj) - GetYDir(pObj) / 3, pObj); 
    if(GetXDir(pObj) > 0) if(GetX(pObj) > GetX() - 6) SetPosition(GetX() - 9, GetY(pObj) - GetYDir(pObj) / 3, pObj); 
    SetXDir(BoundBy(-GetXDir(pObj) / 3, -10, 10), pObj); SetYDir(0, pObj); 
    // Sounds dazu 
    if(GetMass(pObj) >= 10 && HasArmor()) Sound("ClonkHit*"); 
    if(GetMass(pObj) < 10 && HasArmor()) Sound("ArrowRicochet*"); 
    ProtectedCall(pObj,"Hit");       
    return(1); 
  }            
  return(_inherited(pObj,...)); 
} 
 
protected func CatchBlow(iLevel, pObj) 
{ 
  // Zus�tzlicher Schutzfaktor: Weniger Energieverlusst 
  DoEnergy(-iLevel * IsShielded() / 100); 
  return(_inherited(iLevel * (100 - IsShielded()) / 100, pObj));     
} 
 
/* Fallschaden */ 
protected func Hit3() 
{ 
  if (HasArmor()) 
  { 
    DoEnergy(-10); 
    Sound("ClonkHit*");   
  }     
  return(_inherited(...)); 
} 
 
static const KNIG_ShieldLayer=10; // Der Layer der Schildgrafik

/* Schild anziehen */ 
public func WearShield(pObj, szGraphic)
{
  // Nur ein Schild gleichzeitig
  if(HasShield()) return;
  // Kein Objekt oder keine Grafik angegeben?
  if(!pObj || !szGraphic) return;
  // Grafik nicht vorhanden?
  if(!SetGraphics(szGraphic, this(), GetID(), KNIG_ShieldLayer, GFXOV_MODE_ExtraGraphics)) return;
  // Angezogen? dann ID speichern und Objekt l�schen
  idShield = GetID(pObj);
  RemoveObject(pObj);
  // Sound dazu!
  Sound("Connect");
  return(1);
}

/* Schild ausziehen */ 
public func RemoveShield() 
{ 
  // Kein Schild an? 
  if (!idShield) return(0);
  // Sound
  Sound("Connect");
  // Grafikoverlay entfernen
  SetGraphics(0, this, 0, KNIG_ShieldLayer, GFXOV_MODE_ExtraGraphics);
  // Schildobjekt erzeugen und einstecken
  Collect(CreateObject(idShield,0,10,GetOwner()),this);
  idShield=0;
  return(1); 
}    

/* R�stung anziehen */ 
public func WearArmor(pArmor) 
{ 
  // Kein R�stungsobjekt �bergeben? 
  if (!pArmor) return(0); 
  // Schon eine R�stung an? 
  if (fArmored) return(0); 
  // Nicht am Boden?
  if (GetAction() != "Walk") return(0);
  // Neue Grafik setzen 
  if (!SetGraphics(pArmor->~GetGraphics())) 
    // Grafik nicht vorhanden: Sicher ein abgeleitetes Objekt, das keine R�stung tragen kann 
    return(0); 
  // Mit dem Anziehen beginnen 
  SetAction("PutOnArmor");  
  SetComDir(COMD_None);  
  SetXDir(0); 
  // R�stungstyp speichern und R�stungsobjekt verbrauchen 
  idArmorType = GetID(pArmor); 
  RemoveObject(pArmor); 
  // Sound 
  Sound("Connect"); 
  return(1); 
} 
 
/* R�stung ausziehen */ 
public func RemoveArmor() 
{ 
  // Keine R�stung an? 
  if (!fArmored) return(0);
  // Nur im laufen, sonst wird evtl. ein Sprung abgebrochen
  if (GetAction() != "Walk") return(0);
  // Sound 
  Sound("Connect"); 
  // Mit dem Ausziehen beginnen 
  SetAction("PutOffArmor");  
  SetComDir(COMD_None);  
  SetXDir(0); 
  return(1); 
}              
 
/* Callback: R�stung fertig ausgezogen bzw. Anziehen abgebrochen */ 
protected func ArmorRemoved() 
{ 
  // R�stungsobjekt neu erstellen  
  CreateContents(idArmorType);   
  // Ausziehen 
  SetArmored(0); 
}          
 
/* Callback: R�stung fertig angezogen */ 
protected func ArmorPutOn() 
{ 
  // Angezogen 
  SetArmored(1, idArmorType); 
} 
 
/* R�stungsstatus setzen */ 
public func SetArmored(fnArmored, idnArmorType) 
{ 
  // R�stung angelegt 
  if (fArmored = fnArmored) 
  {                   
    // Kein R�stungstyp angegeben! 
    if (!idnArmorType) { fArmored = 0; return(0); } 
    // R�stungstyp speichern 
    idArmorType = idnArmorType;     
    // Grafik setzen 
    var strArmorType = DefinitionCall(idArmorType, "GetGraphics"); 
    SetGraphics(strArmorType); 
    // Beschr�nkte Physicals ...k�nnte man noch von der jeweiligen R�stung abh�ngig machen 
    SetPhysical("Jump", GetPhysical("Jump") * 3 / 4, 2); // Sprungkraft tempor�r vermindern 
    SetPhysical("Swim", GetPhysical("Swim") / 2, 2); // Schwimmen klappt auch nicht mehr so richtig 
    SetPhysical("CanScale", 0, 2);  
    SetPhysical("CanHangle", 0, 2); 
    // Ruestungsportrait 
    var iPortraitCount = 3; 
    if (strArmorType == "BlackKnight") iPortraitCount = 1; // Ein GetPortraitCount("Name") w�re nett...     
    SetPortrait(Format("%s%d", strArmorType, 1 + Random(iPortraitCount)), this, GetID(), 0, 0); 
  } 
  else 
  { 
    // R�stungstyp l�schen 
    idArmorType = 0;     
    // Normale Grafik 
    SetGraphics("");   
    // Normale Physicals 
    ResetPhysical(); 
    // Normales Portrait 
    SetPortrait(GetPortrait(this, 0, 1), this, GetPortrait(this, 1, 1), 0, 0); 
  } 
  // Ok 
  return(1); 
}    
 
protected func CalcValue() 
{ 
  // Standardwert 
  var iValue = _inherited(); 
  // Wert der R�stung hinzurechnen 
  if (fArmored) 
    iValue += GetDefValue(idArmorType); 
  // Wert zur�ckliefern 
  return(iValue);   
} 
 
private func Scaling() 
  { 
  var szDesiredAction; 
  if (GetYDir()>0) szDesiredAction = "ScaleDown"; else szDesiredAction = "Scale"; 
  if (GetAction() != szDesiredAction) SetAction(szDesiredAction); 
  return(1);    
  } 
 
protected func CheckStuck() 
{                    
  // Verhindert Festh�ngen am Mittelvertex 
  if(!GetXDir()) if(Abs(GetYDir()) < 5) 
    if(GBackSolid(0, 3)) 
      SetPosition(GetX(), GetY() + 1); 
} 
 
     
/* Suchfunktionen ======================================================== */ 
 
private func FindEnemyUnit() 
{ 
  var pObj; 
  // N�chstgelegene Mannschaftsobjekte �berpr�fen 
  while (pObj = FindObject(0, 0,0,-1,-1, OCF_CrewMember, 0, 0, 0, pObj)) 
    // Verfeindet? 
    if (Hostile(GetOwner(),GetOwner(pObj))) 
      // Feind gefunden 
      return(pObj); 
  // Keine feindliche Einheit gefunden 
  return(0); 
} 
 
private func FindClosestBase() 
{ 
  // N�chstgelegene Heimatbasis suchen 
  var i, pObj, pClosestBase = 0; 
  for(var i = 0; pObj = FindBase(GetOwner(), i); i++) 
    if (!pClosestBase || ObjectDistance(pObj) < ObjectDistance(pClosestBase)) 
      pClosestBase = pObj; 
  return(pClosestBase); 
}

protected func FindAttachedItems(object after)
{
  var obj = after;
  while(obj = FindObject(0, 0, 0, 0, 0, 0, 0, this(), 0, obj) )
    if(GetProcedure(obj) == "ATTACH")
      // ein Ritterding
      if(obj->~UnbuckledID())
        return(obj);
  return(0);
}
 
/* Bogenschie�en ========================================================== */ 
 
private func AimBowToTarget(iXOffset, iYOffset) 
{ 
  // Richtung 
  if (iXOffset > 0) SetDir(DIR_Right); 
  if (iXOffset < 0) SetDir(DIR_Left); 
  // Zielrichtung 
  var iAngle = Angle(0,0,Abs(iXOffset),iYOffset); 
  SetPhase(BoundBy( iAngle / 23, 0, 5)); 
  // H�her zielen f�r gr��ere Distanz 
  if (GetPhase() > 1) 
    SetPhase(BoundBy(GetPhase() - Abs(iXOffset) / 150, 1, 5)); 
  // Fertig 
  return(1); 
} 
 
private func AimBowToAngle(iAngle) 
  { 
  // Winkel anpassen 
  while (iAngle > 180) iAngle -= 360; 
  // Richtung 
  if (iAngle > 0 ) SetDir(DIR_Right); else SetDir(DIR_Left); 
  // Zielrichtung 
  SetPhase(BoundBy( (Abs(iAngle)+8)/16, 0,7)); 
  // Fertig 
  return(1); 
  } 

private func SimBow(int iAngle, int iTargetX)
{
  // Startwerte
  var iX, iY, iR, iXDir, iYDir, iRDir;
  GetBowOut(0, iAngle, false, iX, iY, iR, iXDir, iYDir, iRDir);
  // Koordinaten umrechnen
  iX += GetX(); iY += GetY();
  // Zeit, die das Geschoss fliegen muss
  var iTime = (iTargetX - iX) * 10 / iXDir;
  // Simulieren
  return(SimFlight(iX, iY, iXDir, iYDir, 1, 100, iTime));
}

private func FireBowAt(int iX, int iY, bool fForce) 
{ 
  var v = 10, // 10 px / tick
      f = 200, // Fixpunktfaktor
      g = -GetGravity() * f * f / 1000 / v, // 0,2 px / tick�
      x = (iX - GetX()) * f / v,
      y = -(iY - GetY()) * f / v /* Korrektur: */ - Abs(x) * GetGravity() / 2000,
      d = y * y + x * x,
      k = y + f * f * f / 2 / g,
      w = k * k - d;
  // Au�er Reichweite?
  if(w < 0 && !fForce) return;
  // Flugzeiten und Winkel berechnen
  var t1 = Sqrt( (k + Sqrt(w)) * (f * f * f / g) ),
      t2 = Sqrt( (k - Sqrt(w)) * (f * f * f / g) ),
      phi1 = ArcCos(x, t1),
      phi2 = ArcCos(x, t2);
  if(y < g * t1 / f * t1 / f / f) phi1 = -phi1;
  if(y < g * t2 / f * t2 / f / f) phi2 = -phi2;
    // Winkel umrechnen
    phi1 = (270 - phi1) % 360 - 180; phi2 = 90 - phi2;
  // Erste L�sung au�erhalb der Grenzen?
    if(!Inside(phi1, -120, 120) && !fForce) return;
    phi1=BoundBy(phi1, -120, 120);
    // Zielwinkel probieren
  var iAngle;
  if(!SimBow(phi1, iX))
    iAngle = phi1;
  // Zweite Schussl�sung?
  else if(t2 < t1 * 3 && !SimBow(phi2, iX))
    iAngle = phi2;
  else if(fForce)
    iAngle = phi1;      
    else
    return;
  // Zielt gar nicht: erst mal laden 
  if (!IsAiming())
    {
    LoadBow(FindContents(BOW1), 1);  
    return;  
    }
  // Zielen 
  AimBowToAngle(iAngle); 
  // Feuern 
  return(Contents()->~FireAngle(iAngle));
} 

/* Beginnt den Zielvorgang */  
public func AimBow() 
{ 
  // Gibt den R�ckgabewert von SetAction zur�ck 
  if (IsRiding()) 
    return(SetAction("RideAimBow")); 
  else 
    return(SetAction("AimBow"));   
}  
  
/* Beginnt den Ladevorgang */  
public func LoadBow(pBow, fFirstTime) 
  { 
  // Nur mit Pfeil! 
  if (!GetArrowCount()) 
    { 
    // Kein Pfeil f�rs erste Zielen: Meldung ausgeben 
    if (fFirstTime) 
      { 
      Sound("KnightFailure"); 
      PlayerMessage(GetController(), "$MsgNoArrow$", this()); // Kein Pfeil 
      } 
    return(1); 
    } 
  // Bogen lieber wieder ausw�hlen (GetArrow w�hlt gerne ab...) 
  ShiftContents(0,0,BOW1); 
  // Schon beim Nachladen? 
  if (IsLoading()) return(1); 
  // War gerade beim Zielen: letzten Winkel sichern 
  if (IsAiming())  
    iLastAimAngle = GetPhase();  
  else  
    iLastAimAngle = 4; 
  // Muss den R�ckgabewert von SetAction zur�ckgeben, da das Setzen durch �berladungen fehlgeschlagen sein kann 
  if (IsRiding()) 
    return(SetAction("RideLoadBow")); 
  else 
    return(SetAction("LoadBow")); 
  } 
 
protected func BeginAiming() { return(SetPhase(iLastAimAngle)); } 

/* Winkel, in den der Ritter momentan zielt */
public func GetBowAimAngle()
  { 
  // Richtung ermitteln 
  var iOutDir; if (!GetDir()) iOutDir=-1; else iOutDir=1; 
  // Winkel (von oben) ermitteln
  return(iOutDir*GetPhase()*115/7);
  }

/* Zielabweichung des Ritters. �berladbar. */
public func GetAimingDeviation(pBow)
  {
  return(5);
  }
  
/* Ausgangsgeschwindigkeit und -position, wenn der Ritter mit dem Bogen in angegebene Richtung schie�t */
public func GetBowOut(pBow, iAngle, fDeviate, &iOutX, &iOutY, &iOutR, &iOutXDir, &iOutYDir, &iOutRDir) 
  {
  // Zufallsabweichung
  if(fDeviate) 
    { 
    var iDeviation = GetAimingDeviation(pBow);
    iAngle += Random(iDeviation * 2 + 1) - iDeviation;
    }
  // Austrittsgeschwindigkeit ermitteln 
  iOutXDir = Sin(iAngle, 70);
  iOutYDir = -Cos(iAngle, 70);
  // Austrittsposition... 
  iOutX = Sin(iAngle, 25 * GetCon() / 100);
  iOutY = -Cos(iAngle, 25 * GetCon() / 100); 
  // Pfeildrehung 
  iOutR = iAngle; 
  if(iAngle > 0) iOutRDir = 3; else iOutRDir = -3;
  // Fertig 
  return(1);
  } 

private func ChangeBowAimAngle(iChange) 
  { 
  var iOldAngle; 
  // Derzeitigen Winkel ermitteln 
  if (IsAiming()) iOldAngle = GetPhase(); 
  if (IsLoading()) iOldAngle = iLastAimAngle; 
  // Am Anschlag? 
  if (!Inside(iOldAngle + iChange, 0, 7)) return; 
  // Neuen Winkel setzen 
  if (IsAiming()) SetPhase(iOldAngle + iChange); 
  if (IsLoading()) iLastAimAngle += iChange; 
  } 
 
public func BowControlUp(pBow, &rVal) 
  { 
  ChangeBowAimAngle(-1); 
  // Funktion �berladen 
  return(1); 
  } 
 
public func BowControlDown(pBow, &rVal) 
  { 
  ChangeBowAimAngle(+1); 
  // Funktion �berladen 
  return(1); 
  } 

/* Aufwertung: Nur erlauben, wenn der Clonk noch kein Ritter (z.B.: K�nig) ist */

public func IsRevaluationBlocked(object target) { return(target->~IsKnight()); }

public func OnRevaluation()
  {
  // Der Magier bekommt bei Aufwertung Mana - Der Ritter also Waffen
  var i = Random(3);
       if (!i--) { CreateContents(BOW1); CreateContents(ARWP); }
  else if (!i--) CreateContents(SWOR);
  else if (!i--) { CreateContents(AXE1); CreateContents(SHIE); }
  }
  
