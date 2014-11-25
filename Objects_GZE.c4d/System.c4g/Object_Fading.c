/*-- Fading --*/

#strict 2

global func SetObjAlpha(int byAlpha,object pObj) {
	var dwClrMod=GetClrModulation(pObj);
	if (dwClrMod == 0) dwClrMod = RGBa(255,255,255,0);

	dwClrMod = SetRGBaValue(dwClrMod,byAlpha,0);
 	return SetClrModulation(dwClrMod, pObj);
}

global func FxFadeOutStart(target, no, temp, level, amount) {
	if(!level) {
		EffectVar(1, target, no) = 255;
	} else {
		EffectVar(1, target, no) = level;
	}

	EffectVar(2, target, no) = Max(1,amount);
}

global func FxFadeOutTimer(target, no) {
	EffectVar(0, target, no)+= EffectVar(2, target, no);
	SetObjAlpha(EffectVar(0, target, no),target);
	
	if (EffectVar(0, target, no) >= EffectVar(1, target, no)) { 
		if (EffectVar(1, target, no) >= 255) {
			RemoveObject(target); 
			return -1; 
		}
	}
}

global func FadeOut(object pObject, int level, bool fFast, int iAmount) {
	if (!pObject) {
		pObject = this;
	}

	if (GetEffect("*FadeOut*", pObject)) {
		return;
	}
	if (fFast) {
		return AddEffect("FadeOut", pObject, 101, 1,0,0,level, iAmount);
	}
	return AddEffect("FadeOut", pObject, 101, 2,0,0,level, iAmount);
}

global func FxFadeInStart(target, no, temp, level, amount) {
	EffectVar(0, target, no) = 255;
	EffectVar(1, target, no) = level;
	EffectVar(2, target, no) = Max(1,amount);
}

global func FxFadeInTimer(target, no) {
	EffectVar(0, target, no)-=EffectVar(2, target, no);
	SetObjAlpha(EffectVar(0, target, no),target);
	
	if (EffectVar(0, target, no) <= EffectVar(1, target, no)) { 
		return -1; 
	}
}

global func FadeIn(object pObject, int level, bool fFast, int iAmount) {
	if (!pObject) {
		pObject = this;
	}
	if (GetEffect("*FadeIn*", pObject)) {
		return;
	}
	if (fFast) {
		return AddEffect("FadeIn", pObject, 101, 1,0,0,level, iAmount);
	}
	return AddEffect("FadeIn", pObject, 101, 2,0,0,level, iAmount);
}

global func StopFading(object pObject) {
	if (!pObject) {
		pObject = this;
	}
	var eff = GetEffectCount("*Fade*",pObject);
	for (eff; eff; eff--) {
		RemoveEffect("*Fade*",pObject);
	}
}
