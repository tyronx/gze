// Work in progress - 
// ShakeViewPort() was originally intended to be additive, but it's broken. trying to fix it for GZE

#strict 2

global func ShakeViewPort(int iLevel, object pObj, int iOffX, int iOffY, bool onlyHorizontal, bool fadein) {
	if(iLevel <= 0) return(false);
	
	var eff=GetEffect("ShakeEffect",pObj);
	
	if(eff) {
		//Log("already have effect, adding my strength to it");
		EffectVar(0,pObj,eff)+=iLevel;
		return(true);
	}
	
	eff=AddEffect("ShakeEffect",pObj,200,1);
	
	if (!eff) {
		return(false);
	}
	
	EffectVar(0,pObj,eff)=iLevel;
	EffectVar(3,pObj,eff)=onlyHorizontal;
	EffectVar(4,pObj,eff)=fadein;
	
	
	if(iOffX || iOffY) {
		EffectVar(1,pObj,eff)=iOffX;
		EffectVar(2,pObj,eff)=iOffY;
	} else {
		EffectVar(1,pObj,eff)=GetX(pObj);
		EffectVar(2,pObj,eff)=GetY(pObj);
	}
	return (true);
}

// Dauer des Effektes: sobald iStrength==0
// Stärke des Effektes: iStrength=iLevel/(1.5*iTime+3)-iTime^2/400
global func FxShakeEffectTimer(object pTarget, int iEffectNumber, int iTime) {
	var iPlr, iLevel;
	var iStrength;
	
	for (var i=0; i<GetPlayerCount(); i++) {
		iPlr = GetPlayerByIndex(i);
		// Schütteleffekt verringert sich je nach Entfernung
		iLevel=
			(300*EffectVar(0,pTarget,iEffectNumber)) / Max(300,Distance(GetX(GetCursor(iPlr)),GetY(GetCursor(iPlr)),EffectVar(1,pTarget,iEffectNumber),EffectVar(2,pTarget,iEffectNumber)));
		
		if ((iStrength= iLevel/((3*iTime)/2+3)-iTime**2/400) <= 0) {
			continue;
		}
		
		if (EffectVar(3,pTarget,iEffectNumber)) {
			// FIXME: Use GetViewOffset, make this relative, not absolute
			SetViewOffset(iPlr,Sin(iTime*100,iStrength),0);
		} else {
			SetViewOffset(iPlr,Sin(iTime*100,iStrength),Cos(iTime*100,iStrength));
		}
	}
	if(EffectVar(0,pTarget,iEffectNumber)/((3*iTime)/2+3)-iTime**2/400<=0) {
		return(-1);
	}
}

global func FxShakeEffectStart(object pTarget, int iEffectNumber) {
  FxShakeEffectTimer(pTarget, iEffectNumber, GetEffect (0, pTarget, iEffectNumber, 6));
}

global func FxShakeEffectStop() {
  for(var i=0; i<GetPlayerCount(); i++) {
    // FIXME: Return the offset to the previous value, not zero
    SetViewOffset(GetPlayerByIndex(i),0,0);
  }
}