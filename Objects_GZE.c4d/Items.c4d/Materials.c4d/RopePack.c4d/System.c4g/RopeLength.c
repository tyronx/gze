/*-- Seillänge bei Betreten/Verlassen von Objekten anpassen --*/

#strict 2
#appendto *

func RejectEntrance(pIntoObj)
{
  for(var pRope in FindObjects(Find_ID(CK5P)))
  {
    if(GetActionTarget(1, pRope) == this || GetActionTarget(0, pRope) == this)
    {
      var iDistance = ObjectDistance(this, pIntoObj);
      pRope->SetRopeLength(pRope->GetRopeLength() + iDistance);
      return _inherited(pIntoObj, ...);
    }
    for(var pObj in FindObjects(Find_AnyContainer()))
    {
      if(GetRealContainer(pObj) == this)
      {
        if(GetActionTarget(1, pRope) == pObj || GetActionTarget(0, pRope) == pObj)
        {
          var iDistance = ObjectDistance(this, pIntoObj);
          pRope->SetRopeLength(pRope->GetRopeLength() + iDistance);
        }
      }
    }
  }
  return _inherited(pIntoObj, ...);
}

func Departure(pContainer)
{
  for(var pRope in FindObjects(Find_ID(CK5P)))
    if(GetActionTarget(1, pRope) == this || GetActionTarget(0, pRope) == this)
    {
      var iDistance = ObjectDistance(this, pContainer);
      pRope->SetRopeLength(pRope->GetRopeLength() + iDistance);
    }
  return _inherited(pContainer, ...);
}
