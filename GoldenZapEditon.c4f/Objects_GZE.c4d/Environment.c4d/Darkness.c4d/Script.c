/*-- Düsternis --*/

#strict 2

//Ob es wirklich dunkel ist, seht ihr wenn das licht ausgeht.
local darkness;

// Ursprüngliche Himmelsfarben
static DarknessSkyAdjustOrig;
//static DarknessMatAdjustOrig;

// Kann mittels des Spielzielauswählers ausgewählt werden
//public func IsChooseable() { return(true); }
// Das war einmal. Der Regelwähler behandelt Dunkelheit jetzt gesondert.

protected func Activate(iPlr) {
	MessageWindow(GetDesc(),iPlr);
}

public func Initialize() {
	ScheduleCall(this(),"PostInitialize",1);
}

private func PostInitialize()	{
	//nur eine Dunkelheit!
	var count = 1;
	for (var d in FindObjects(Find_ID(GetID()),Find_Exclude(this()))) {
		count++;
		RemoveObject(d);
	}

	//sehr wichtig
	if (!GetDarkness(1000))
		SetDarkness(count);
}

public func UpdateLights() {
	var obj;
	
	for(obj in FindObjects(Find_Func("IsLight"))) {
		obj->~Update();
	}
	
	for(obj in FindObjects(Find_OCF(OCF_CrewMember))) {
		obj -> SetPlrViewRange(CalcViewRange());
	}
}

private func CalcViewRange() {
	var iMax = GameCall("MaxPlrViewRange");
	if (!iMax) {
		iMax = 700;
	}
	return (700-6*GetDarkness(100))*iMax/700;
}

global func CalcLight(&alphamod, &sizemod) {
	sizemod = 100+GetDarkness(1000)*3/2/10; // bis zu 250% so groß
	alphamod = (1000-GetDarkness(1000))/50; // 0-20 alpha werden aufaddiert
	
	// keine Dunkelheit: beinahe unsichtbar
	// Genauso bei Dunkelheit = 0
	if (!IsDark() || !GetDarkness(1000)) {
		alphamod = 30;
	}
}

public func InitializePlayer(int iPlr) {
	SetFoW(true,iPlr);
}

public func OnClonkRecruitment(object pClonk) {
	if (ObjectCount(GetID()) > 1) {
		return(ScheduleCall(this(),"OnClonkRecruitment",1,0,pClonk));
	}
	SetPlrViewRange(CalcViewRange(),pClonk);
	var tmp = AddLightAmbience(80,pClonk);
	tmp->ChangeColor(RGBa(255,255,255,110));
	SetVisibility(VIS_Owner,tmp);
}

global func IsDark() {
	var obj;
	if (GetID(this()) != DARK) {
		obj = FindObject(DARK);
	} else {
		obj = this();
	}
	//kein Dunkelheit-Objekt -> Keine Dunkelheit
	if(!obj) {
		return(false);
	}
	return(obj);
}

global func GetDarkness(int precision) {
	var dark;
	if(!(dark = IsDark()))
		return(0);

	if(!precision) precision = 10;	

	var val;
	
	var darkness = dark->LocalN("darkness");
	
	val = darkness/(1000/precision);
		
	return(val);
}

global func SetDarkness(int iGrade) {
	var obj;
	if(GetID(this()) != DARK) {
		obj = FindObject(DARK);
	} else {
		obj = this();
	}
	
	//kein Dunkelheit-Objekt -> Keine Dunkelheit
	if(!obj) {
		return(false);
	}

	iGrade = BoundBy(iGrade,0,100);
	
	if(GetType(DarknessSkyAdjustOrig) != C4V_Array) {
		DarknessSkyAdjustOrig = [GetSkyAdjust(), GetSkyAdjust(true)];
	}

	obj->LocalN("darkness") = iGrade*10;

	DarknessApplyGrade(iGrade);
	
	obj->UpdateLights();
	
	return(true);
}

global func SetSkyAdjust(int clr, int backclr) {
	if(GetID() != DARK) {
		DarknessSkyAdjustOrig = [clr, backclr];
	}
	return inherited(clr, backclr, ...);
}

// iStep: wie viel Änderung der Dunkelheit pro 10 Frames
global func FadeDarkness(int iGrade, int iStep) {
	var obj;
	if(GetID(this()) != DARK)
		obj = FindObject(DARK);
	else
		obj = this();
	//kein Dunkelheit-Objekt -> Keine Dunkelheit
	if(!obj)
		return(false);

	if(!iStep) iStep = 100;

	iGrade = BoundBy(iGrade,0,100);
	
	var darkness = obj->LocalN("darkness");
	
	AddEffect("Fading",obj,20,1,obj,DARK,iGrade*10,darkness,iStep);
	
	return(true);
}

public func Destruction() {
	SetDarkness(0);
	UpdateLights();
	for (var light in FindObjects(Find_Func("IsLight"),Find_Func("LocalN(\"bAmbience\")"))) {
		RemoveObject(light);
	}
}

func FxFadingStart(object pTarget, int iEffectNumber, int iTemp, int endgrade, int startgrade, int frames) {
	if(iTemp) {
		return;
	}
	EffectVar(0, pTarget, iEffectNumber) = endgrade;
	EffectVar(1, pTarget, iEffectNumber) = startgrade;
	EffectVar(2, pTarget, iEffectNumber) = frames;	
	EffectVar(3, pTarget, iEffectNumber) = 0;	
}

func FxFadingTimer(object pTarget, int iEffectNumber, int iEffectTime) {
	var grade = EffectVar(1,pTarget,iEffectNumber);
	var end = EffectVar(0,pTarget,iEffectNumber);
	var fade = EffectVar(2,pTarget,iEffectNumber);
	var go = EffectVar(3,pTarget,iEffectNumber);
	if (grade == end) {
		return(-1);
	}
	go += fade;

	if (go >= 10) {
		if(grade > end) {
			grade = Max(grade-go/10, end);
		}
		else {
			grade = Min(grade+go/10, end);
		}
		go = 0;
		
		EffectVar(1,pTarget,iEffectNumber) = grade;

		//var g = BoundBy(128-grade*100/1000,0,128);
		//var m = BoundBy(255-grade*100/1000,0,255);
	
		DarknessApplyGrade(grade);
		pTarget->LocalN("darkness") = grade;
		UpdateLights();
	}

	EffectVar(3,pTarget,iEffectNumber) = go;

}

func FxFadingEffect(string szNewEffectName, object pTarget, int iEffectNumber, int iNewEffectNumber, int endgrade, int stargrade, int frames) {
	if(szNewEffectName == "Fading")
		return(-3);
}

func FxFadingAdd(object pTarget, int iEffectNumber, string szNewEffectName, int iNewEffectTimer, int endgrade, int stargrade, int frames) {
	EffectVar(0,pTarget,iEffectNumber) = endgrade;
	EffectVar(2,pTarget,iEffectNumber) = frames;
}



global func DarknessApplyGrade( int grade ) {
	grade = BoundBy(grade, 0, 100);
	
	/*var dayHueShift = [0,0,0];
	var nightHueShift = [0, -50, -50];*/
	
	var gammaLight = ((100-grade/4) * 255) / 100;
	var skyLight =  ((100-grade) * 255) / 100;
	var matLight = BoundBy(80 + ((100-grade) * 255) / 100, 0, 255);
	
	SetGamma(RGB(0,0,0),HSL(0, grade/2, gammaLight/2), HSL(0,grade/2, gammaLight), 2);
	
	DARK->SetSkyAdjust(
		SetLFromRGBa(DarknessSkyAdjustOrig[0], skyLight),
		RGB(1,1,1)
	);
	DARK->SetMatAdjust(
		SetLFromRGBa(DarknessSkyAdjustOrig[0], matLight)
	);
	
	//Log("%X", rgba);
	
	/*
	SetGamma(RGB(0,0,0),HSL(g,g,64+g/2),RGB(255,255,255),2);
	
	DARK -> SetSkyAdjust(
		RGBa(
			(127+g) * GetRGBValue(DarknessSkyAdjustOrig[0],1) / 255,
			(127+g) * GetRGBValue(DarknessSkyAdjustOrig[0],2) / 255,
			(127+g) * GetRGBValue(DarknessSkyAdjustOrig[0],3) / 255,
			BoundBy(255-g,0,255)
		),
		RGB(1,1,1)
	);

	DARK -> SetMatAdjust(RGB(
		BoundBy(127+g,0,255) * GetRGBValue(DarknessSkyAdjustOrig[0],1) / 255,
		BoundBy(127+g,0,255) * GetRGBValue(DarknessSkyAdjustOrig[0],2) / 255,
		BoundBy(127+g,0,255) * GetRGBValue(DarknessSkyAdjustOrig[0],3) / 255 )
	);*/

}