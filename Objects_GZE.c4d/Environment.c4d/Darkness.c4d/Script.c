/*-- Düsternis --*/

#strict 2

static darkness_object;

static const g_DARK_MaxDarkness = 1000;
static const g_DARK_Default_MaxDarkness = 55;

////////////////////////////////////////////////////////////////////
//
// Das Umweltobjekt mit Info-Menus

local darkness;

protected func Activate(iPlr)
{
	MessageWindow(GetDesc(),iPlr);
}

public func Initialize()
{
	ScheduleCall(this, "PostInitialize", 1);
}

private func PostInitialize()
{
	// nur eine Dunkelheit!
	var count = 1;
	for (var d in FindObjects(Find_ID(GetID()),Find_Exclude(this)))
	{
		count++;
		RemoveObject(d);
	}

	// einfach mal speichern
	if (!darkness_object) darkness_object = this;

	// originale Himmelsfarbe - wird auch im Zeit-Objekt gemacht, aber vllt hat man auch mal
	// die Düsternis ohne Zeit?
	if (!original_sky_dword) original_sky_dword = GetSkyAdjust(false);
	if (!original_skybg_dword) original_skybg_dword = GetSkyAdjust(true);
	if (!original_mat_dword) original_mat_dword = RGBa(255,255,255,0);


	// sehr wichtig
	if (!GetDarkness(g_DARK_MaxDarkness))
		SetDarkness(count);
}

public func InitializePlayer(int iPlr)
{
	SetFoW(true,iPlr);
}

// jeder Clonk kriegt gratis einen Lichtschein, gratis!!1
public func OnClonkRecruitment(object pClonk)
{
	if (ObjectCount(GetID()) > 1)
	{
		return ScheduleCall(this,"OnClonkRecruitment", 1, 0, pClonk);
	}

	SetPlrViewRange(CalcViewRange(), pClonk);
	var tmp = LIGH->AddLightAmbience(80, pClonk);

	tmp->ChangeColor(RGBa(255,253,135,110)); // ein bisschen wärmer...

	SetVisibility(VIS_Owner, tmp);
}

////////////////////////////////////////////////////////////////////
//
// Globale Funktionen zur Steuerung des Objekts


global func IsDark()
{
	//kein Dunkelheit-Objekt -> Keine Dunkelheit
	if(!darkness_object) return(false);
	return darkness_object;
}

global func GetDarkness(int precision)
{
	if(!darkness_object) return 0;
	if(!precision) precision = 10;

	var val;

	var darkness = darkness_object->LocalN("darkness");

	val = darkness/(g_DARK_MaxDarkness/precision);

	return val;
}

// Skaliert den Grad relativ zu maximaler und minimaler Dunkelheit.
// D.h. bei MaxDarkness = 70 machen die Befehle folgendes:
//   SetDarkness(50) setzt die Dunkelheit auf 50%
//   SetDarkness(DarknessGradeRelative(50)) setzt die Dunkelheit auf 35%
//
// Ist zusätzlich MinDarkness = 20, dann
//   SetDarkness(DarknessGradeRelative(50)) setzt die Dunkelheit auf 45%
global func DarknessGradeRelative(int iGrade)
{
	var iMin = GameCall("MinDarkness"), iMax = GameCall("MaxDarkness");
	if(!iMax) iMax = g_DARK_Default_MaxDarkness;
	return BoundBy(iGrade, 0, 100) * (iMax - iMin) / 100 + iMin;
}

global func GetDarknessGradeAbs()
{
	return GetDarkness(100);
}

global func GetDarknessGradeRel()
{
	var iMin = GameCall("MinDarkness"), iMax = GameCall("MaxDarkness");
	if(!iMax) iMax = g_DARK_Default_MaxDarkness;

	return 100 * (GetDarkness(100) - iMin) / (iMax - iMin);
}

global func SetDarkness(int iGrade)
{
	if(!darkness_object) return false;

	var grade = BoundBy(iGrade, 0, 100);

	darkness_object->LocalN("darkness") = 10 * grade;
	DarknessApplyGrade(grade);
	darkness_object->UpdateLights();

	return true;
}

global func UpdateDarkness()
{
	if(!darkness_object) return false;
	DarknessApplyGrade(darkness_object->LocalN("darkness"));
}

global func SetSkyAdjust(int clr, int backclr)
{
	if(GetID() != DARK && GetID() != TIME)
	{
		if (!original_sky_dword) original_sky_dword = clr;
		if (!original_skybg_dword) original_skybg_dword = backclr;
	}
	return inherited(clr, backclr, ...);
}

global func SetMatAdjust(int clr)
{
	if(GetID() != DARK && GetID() != TIME)
	{
		if (!original_mat_dword) original_mat_dword = clr;
	}
	return inherited(clr, ...);
}

global func DarknessApplyGrade( int grade )
{
	grade = BoundBy(grade, 0, 100);

	/*var dayHueShift = [0,0,0];
	var nightHueShift = [0, -50, -50];*/

	var color = original_sky_dword;
	if (time_object) color = time_sky_dword;

//	var skyLBefore = GetLFromRGBa(color); // Die Düsternis hatte den Himmel heller gemacht...
//	var matLBefore = GetLFromRGBa(original_mat_dword);
//
//	var gammaLight = ((100-grade/4) * 255) / 100;
//	var skyLight =  ((100-grade) * skyLBefore) / 100;
//	var matLight = BoundBy(80 + ((100-grade) * matLBefore) / 100, 0, 255);
//
//	SetGamma(RGB(0,0,0),HSL(0, grade/2, gammaLight/2), HSL(0,grade/2, gammaLight), GAMMA_Ramp_User1);
//	DARK->SetSkyAdjust(
//		SetLFromRGBa(color, skyLight),
//		RGB(1,1,1)
//	);
//	DARK->SetMatAdjust(
//		SetLFromRGBa(original_mat_dword, matLight)
//	);

	var gammaLight = ((100-grade/4) * 255) / 100;
	var skyLight =  ((100-grade) * 255) / 100;
	var matLight = BoundBy(80 + ((100-grade) * 255) / 100, 0, 255);

	SetGamma(RGB(0,0,0),HSL(0, grade/2, gammaLight/2), HSL(0,grade/2, gammaLight), GAMMA_Ramp_User1);

	var adjustedSky = SetVFromRGBa(color, skyLight);
	var adjustedMat = SetVFromRGBa(original_mat_dword, matLight);
//	Log("darkness color: %d %d %d %d - grade %d", GetRGBaValue(adjustedSky,1), GetRGBaValue(adjustedSky,2), GetRGBaValue(adjustedSky,3), GetRGBaValue(adjustedSky,0), grade);

	DARK->SetSkyAdjust(adjustedSky,RGB(1,1,1));
	DARK->SetMatAdjust(adjustedMat);

	//Log("%X", rgba);
}

global func SetVFromRGBa(int rgba, int newVal)
{
	newVal = BoundBy(newVal, 0, 255);

	var r, g, b, a;
	SplitRGBaValue(rgba, r, g, b, a);
	return RGBa( r * newVal / 255,
			     g * newVal / 255,
			     b * newVal / 255,
			     a);
}

global func CalcLight(&alphamod, &sizemod)
{
	sizemod = 100+GetDarkness(g_DARK_MaxDarkness)*3/2/10; // bis zu 250% so groß
	alphamod = (g_DARK_MaxDarkness-GetDarkness(g_DARK_MaxDarkness))/50; // 0-20 alpha werden aufaddiert

	// keine Dunkelheit: beinahe unsichtbar
	// Genauso bei Dunkelheit = 0
	if (!IsDark() || !GetDarkness(g_DARK_MaxDarkness))
	{
		alphamod = 30;
	}
}

////////////////////////////////////////////////////////////////////
//
// Sonstiges

public func UpdateLights()
{
	var obj;

	for(obj in FindObjects(Find_Func("IsLight")))
	{
		obj->~Update();
	}

	var viewRange = CalcViewRange();
	for(obj in FindObjects(Find_OCF(OCF_CrewMember)))
	{
		obj -> SetPlrViewRange(viewRange);
	}
}

private func CalcViewRange()
{
	var iMax = GameCall("MaxPlrViewRange");
	if (!iMax)
	{
		iMax = 700;
	}
	return (700-6*GetDarkness(100))*iMax/700;
}

public func Destruction()
{
	SetDarkness(0);
	UpdateLights();
	for (var light in FindObjects(Find_Func("IsLight"),Find_Func("LocalN(\"bAmbience\")")))
	{
		RemoveObject(light);
	}
}


//// iStep: wie viel Änderung der Dunkelheit pro 10 Frames
//global func FadeDarkness(int iGrade, int iStep) {
//	var obj;
//	if(GetID(this()) != DARK)
//		obj = FindObject(DARK);
//	else
//		obj = this();
//	//kein Dunkelheit-Objekt -> Keine Dunkelheit
//	if(!obj)
//		return(false);
//
//	if(!iStep) iStep = 100;
//
//	iGrade = BoundBy(iGrade,0,100);
//
//	var darkness = obj->LocalN("darkness");
//
//	AddEffect("Fading",obj,20,1,obj,DARK,iGrade*10,darkness,iStep);
//
//	return(true);
//}
//
//
//
//func FxFadingStart(object pTarget, int iEffectNumber, int iTemp, int endgrade, int startgrade, int frames) {
//	if(iTemp) {
//		return;
//	}
//	EffectVar(0, pTarget, iEffectNumber) = endgrade;
//	EffectVar(1, pTarget, iEffectNumber) = startgrade;
//	EffectVar(2, pTarget, iEffectNumber) = frames;
//	EffectVar(3, pTarget, iEffectNumber) = 0;
//}
//
//func FxFadingTimer(object pTarget, int iEffectNumber, int iEffectTime) {
//	var grade = EffectVar(1,pTarget,iEffectNumber);
//	var end = EffectVar(0,pTarget,iEffectNumber);
//	var fade = EffectVar(2,pTarget,iEffectNumber);
//	var go = EffectVar(3,pTarget,iEffectNumber);
//	if (grade == end) {
//		return(-1);
//	}
//	go += fade;
//
//	if (go >= 10) {
//		if(grade > end) {
//			grade = Max(grade-go/10, end);
//		}
//		else {
//			grade = Min(grade+go/10, end);
//		}
//		go = 0;
//
//		EffectVar(1,pTarget,iEffectNumber) = grade;
//
//		//var g = BoundBy(128-grade*100/1000,0,128);
//		//var m = BoundBy(255-grade*100/1000,0,255);
//
//		DarknessApplyGrade(grade);
//		pTarget->LocalN("darkness") = grade;
//		UpdateLights();
//	}
//
//	EffectVar(3,pTarget,iEffectNumber) = go;
//
//}
//
//func FxFadingEffect(string szNewEffectName, object pTarget, int iEffectNumber, int iNewEffectNumber, int endgrade, int stargrade, int frames) {
//	if(szNewEffectName == "Fading")
//		return(-3);
//}
//
//func FxFadingAdd(object pTarget, int iEffectNumber, string szNewEffectName, int iNewEffectTimer, int endgrade, int stargrade, int frames) {
//	EffectVar(0,pTarget,iEffectNumber) = endgrade;
//	EffectVar(2,pTarget,iEffectNumber) = frames;
//}
//
//
//
