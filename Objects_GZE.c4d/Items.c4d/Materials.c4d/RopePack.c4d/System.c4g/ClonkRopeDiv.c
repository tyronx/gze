/*-- Diverse Erweiterungen des Clonks zur Verwendung des Seils --*/

#strict 2

#appendto CLNK

local pCapturer;


/*-------------------------*/
/*-- Abnehmen von Seilen --*/
/*-------------------------*/

protected func ControlDigDouble(pClonk) {
  if(GetAction(pClonk) == "Push") return _inherited(pClonk, ...);
  // Eklige Effekte bei Mischung mit Leitungsbausätzen... Finger weg!
  if(GetID(Contents()) == LNKT) return _inherited(pClonk, ...);
  // Activate-Funktionen in Inventarobjekten haben Vorrang
  if(Contents() && Contents()->~Activate(this)) return 1;
  // Nur abnehmen, wenn noch ein Inventarplatz frei
  if(ContentsCount() >= this->~MaxContentsCount()) return _inherited(pClonk, ...);
  if(GetDefCoreVal("CollectionLimit", "DefCore", GetID()))
    if(ContentsCount() >= GetDefCoreVal("CollectionLimit", "DefCore", GetID()))
      return _inherited(pClonk, ...);

  for(var pRope in FindObjects(Find_ID(CK5P)))
  {
    for(var pObj in FindObjects(Find_Or(Find_AtPoint(), Find_InRect(-10, -15, 20, 30)), Find_Not(Find_ID(CL5P)), Find_Not(Find_And(Find_AnyContainer(), Find_Not(Find_Container(this))))))
    {
      if(GetActionTarget(1, pRope) == pObj || GetActionTarget(0, pRope) == pObj)
      {
        // Bestimmte Seile können nicht abgenommen werden (z.B. Enterhaken, Anker)
        if((LocalN("fNoPickUp_0", pRope) && GetActionTarget(0, pRope) == pObj)
        || (LocalN("fNoPickUp_1", pRope) && GetActionTarget(1, pRope) == pObj))
          continue;
        // Gefesselt? Keine Selbstbefreiung
        if(pObj == this && GetEffectCount("IntTied", this) && EffectVar(0, this, GetEffect("IntTied", this)) == pRope)
          return _inherited(pClonk, ...);
        Sound("ArrowHit");
        // Seilabnahme von Seilrolle würde zu zusammenfügen des Seiles führen
        if(GetID(pObj) == CL5P) return pObj->PickUp();
        // Seil von Objekt auf Seilrolle umswitchen
        var pPulley = CreateObject(CL5P, 0, 0, GetOwner(this));
        LocalN("pRope", pPulley) = pRope;
        // eventuell Fesselung durch dieses Seil im zuvor verbundenen Objekt aufheben
        for(var i = 0; i < GetEffectCount("IntTied", pObj); i++)
          if(EffectVar(0, pObj, GetEffect("IntTied", pObj, i)) == pRope)
            ResetTiedClonk(pObj, i);
        if(GetActionTarget(1, pRope) == pObj)
        {
          pRope->SetObject(pPulley, 1);
          LocalN("pConnected", pPulley) = GetActionTarget(0, pRope);
        }
        else
        {
          pRope->SetObject(pPulley, 0);
          LocalN("pConnected", pPulley) = GetActionTarget(1, pRope);
        }          
        pPulley->Enter(this);
        return 1;
      }
    }
  }
  return _inherited(pClonk, ...);
}


/*--------------------------------------------*/
/*-- Fesseleffekte verschwinden mit dem Tod --*/
/*--------------------------------------------*/

protected func Death(int iKilledBy)
{
  // War gefesselt? Für Auswertung in OnClonkDeath Effekt noch mal wiederherstellen
  if(FindObject2(Find_ID(CY5P), Find_ActionTarget(this)))
    AddEffect("IntTied", this, 25);
  // Effekt erst NACH OnClonkDeath löschen
  ScheduleCall(0, "RemoveTiedEffects", 1);
  return _inherited(iKilledBy, ...);
}

private func RemoveTiedEffects()
{
  while(GetEffect("IntTied", this))
    RemoveEffect("IntTied", this);
  var pSymbol;
  while(pSymbol = FindObject2(Find_ID(CY5P), Find_ActionTarget(this)))
    pSymbol->RemoveObject();
}


/*-----------------------------------------------------------------*/
/*-- Abfrage des Clonks, der einen anderen zuletzt gefesselt hat --*/
/*-----------------------------------------------------------------*/

global func GetCapturer(pClonk)
{
  if(!pClonk && LocalN("pCapturer", this))
    return LocalN("pCapturer", this);
  if(pClonk && LocalN("pCapturer", pClonk))
    return LocalN("pCapturer", pClonk);
  return 0;
}


/*--------------------------------------------------------*/
/*-- Enterhaken können auch im Springen geworfen werden --*/
/*--------------------------------------------------------*/

protected func ControlThrow(pClonk)
{
  if((GetAction() == "Jump" || GetAction() == "Dive")
  && Contents()
  && Contents()->~IsHook()
  && !GetPlrDownDouble(GetOwner()))
    Throwing();
  return _inherited(pClonk, ...);
}

// Launch über Ejection im Clonk statt Departure im Enterhaken, so verschwinden Zugeffekte (?!)
func Ejection(pObj)
{
  if(pObj->~IsHook()) pObj->~Launch(this);
  return _inherited(pObj, ...);
}


/*---------------------------------------------*/
/*-- Steuerung an Inhaltsobjekte weitergeben --*/
/*--      (zum Klettern mit Seilrollen)      --*/
/*---------------------------------------------*/

protected func ControlUp(pClonk)
{
  if(ObjectCall(Contents(), "ControlUp", this))
    return 1;


/*-----------------------------------*/
/*-- Seilrolle hemmt keine Sprünge --*/
/*-----------------------------------*/

  if(GetID(Contents()) == CL5P && GetProcedure() == "WALK")
  {
    var pRope = LocalN("pRope", Contents());
    if(LocalN("iLength", pRope) - LocalN("iCalcedLength", pRope) < 20)
      LocalN("iLength", pRope) += 20;
  }

  return _inherited(pClonk, ...);
}

protected func ControlDown(pClonk)
{
  if(ObjectCall(Contents(), "ControlDown", this))
    return 1;
  return _inherited(pClonk, ...);
}

protected func ControlLeft(pClonk)
{
  if(ObjectCall(Contents(), "ControlLeft", this))
    return 1;
  return _inherited(pClonk, ...);
}

protected func ControlRight(pClonk)
{
  if(ObjectCall(Contents(), "ControlRight", this))
    return 1;
  return _inherited(pClonk, ...);
}

protected func ControlDigDouble(pClonk)
{
  if(ObjectCall(Contents(), "ControlDigDouble", this))
    return 1;
  return _inherited(pClonk, ...);
}
