/*-- Tageszeiten --*/

#strict 2

/* Konstanten */

// Die Gamme-Konstanten aus der Doku. Das sollte in System.c4g-Script!

static const GAMMA_Ramp_Global = 		0; // Szenarienglobaler Wert
static const GAMMA_Ramp_Climate = 		1; // Klima/Jahreszeiten (Engineintern benutzt, wenn im Szenario aktiviert!)
static const GAMMA_Ramp_User1 = 		2; // frei
static const GAMMA_Ramp_DayNight = 		3; // Tag/Nacht
static const GAMMA_Ramp_User2 = 		4; // frei
static const GAMMA_Ramp_Lightning = 	5; // Blitze
static const GAMMA_Ramp_Effects = 		6; // Zaubereffekte
static const GAMMA_Ramp_User3 =		 	7; // frei


static const g_TIME_BaseSpeed_SecondsPerTick = 8;

static const g_TIME_Day_Seconds = 86400;
static const g_TIME_Hour_Seconds = 3600;
static const g_TIME_Minute_Seconds = 60;

static const g_TIME_TickInterval_Frames = 10;

static const g_TIME_YearLength = 20; // so viele Tage dauert ein Jahr

static const g_TIME_Oldschool = false; // der alte CR Modus?

/* Variablen */

static time_years, time_days, time_hours, time_minutes;
static time_hours_old;
static time_object;

local original_sky_dword; // So sah der Himmel vorher aus
local original_sky_array; // und jetzt nochmal als Farb-Array [r,g,b,a]

local time; // Aktuelle Zeit, in Sekunden
local advance_seconds_per_tick; // So viele Sekunden vergehen je Tick
local light_intensity; // Wie hell/dunkel es ist, in Prozent -> wird das überhaupt benötigt??


////////////////////////////////////////////////////////////////////
//
// Engine Zeug und Organisatorisches

protected func Initialize()
{
	advance_seconds_per_tick = g_TIME_BaseSpeed_SecondsPerTick;
	light_intensity = 100;

	time = Time( 12, 0, 0);

	//	if(!ObjectCount2(Find_ID(DARK)))
	//	{
	//		CreateObject(DARK, 0, 0, NO_OWNER);
	//	}
	//	SetDarkness(GameCall("MinDarkness"));

	SetAction("Initialize");
}

private func Initialized()
{
	advance_seconds_per_tick = g_TIME_BaseSpeed_SecondsPerTick * (ObjectCount(TIME) + 1);

	// Andere Objekte des gleichen Typs entfernen
	var pOther;
	while(pOther = FindObject(GetID()))
		RemoveObject(pOther);

	// Jetzt aber mal komfortabel das Objekt speichern...
	time_object = this;

	// Himmelsmodulation speichern - geht nur mit nicht-transparentem Himmel, momentan
	original_sky_dword = GetSkyAdjust();
	original_sky_array = [ GetRGBaValue(original_sky_dword, 1),
	                       GetRGBaValue(original_sky_dword, 2),
	                       GetRGBaValue(original_sky_dword, 3),
	                       GetRGBaValue(original_sky_dword, 0)];

	Log("Time speed is now %d", advance_seconds_per_tick);

	AddEffect("IntTimeAdvance", this, 1, g_TIME_TickInterval_Frames, this);
}

private func FxIntTimeAdvanceTimer()
{
	DoTimeProgress(true);
	DoSkyShade();
}

protected func UpdateTransferZone()
{
	// Kleiner Trick, damit der Himmel nach dem Laden aktualisiert wird
	if (GetAction() == "Time") FxIntTimeAdvanceTimer();
}

////////////////////////////////////////////////////////////////////
//
// Die eigentlichen Effekte

// Zählt die Tage, Stunden, usw.
private func DoTimeProgress(bool updateDays)
{
	// Und weiter...
	time += advance_seconds_per_tick;

	// Tage zählen
	while (updateDays && time >= g_TIME_Day_Seconds)
	{
		time_days++;
		time -= g_TIME_Day_Seconds;
	}

	while (time_days >= g_TIME_YearLength)
	{
		time_days -= g_TIME_YearLength;
		time_years++;
	}

	time_hours = time / g_TIME_Hour_Seconds;
	time_minutes = (time - time_hours * g_TIME_Hour_Seconds)/g_TIME_Minute_Seconds;

	// Alle Objekte wissen lassen, dass sich die Zeiten ändern ^^
	if( time_hours != time_hours_old )
	{
		var obj;
		for(obj in FindObjects(Find_Func("UpdateTime", time_hours, time_hours_old)));

		time_hours_old = time_hours;
	}
}


// By Ringwaul
// Macht tolle Farben.
private func DoSkyShade()
{
	// first determine the time phase we are in
	var sunrise, sunset, night, day;

	sunrise = IsDawn();
	day = IsDay();
	sunset = IsDusk();
	night = IsNight();

	var skyshade = [0,0,0,0]; //R,G,B,A
	var nightcolour = [10,25,40]; // default darkest-night colour
	var daycolour = original_sky_array;//[255,255,255];
	var sunsetcolour = [140,45,10];
	var sunrisecolour = [140,100,70];

	var sunrise_start = GetDawnPeriod()[0];
	var sunrise_end = GetDawnPeriod()[1];
	var sundown_start = GetDuskPeriod()[0];
	var sundown_end = GetDuskPeriod()[1];

//	if (!day)
//	{
//		// Darkness of night dependent on the moon-phase
//		var satellite = FindObject(Find_ID(MOON));
//		if(satellite)
//		{
//			var lightness = satellite->GetMoonLightness();
//			nightcolour = [ 6 * lightness / 100, 8 + 25 * lightness / 100, 15 + 60 * lightness / 100 ];
//		}
//	}

	// Sunrise
	if (sunrise)
	{
		var time_since_sunrise = time - sunrise_start;
		// progress in 0..1800
		var progress = time_since_sunrise * 1800 / (sunrise_end - sunrise_start);

		for(var i=0; i<3; ++i)
		{
			var nightfade = Cos(progress/2, nightcolour[i],10);
			var dayfade = daycolour[i] - Cos(progress/2, daycolour[i],10);
			var sunrisefade = Sin(progress, sunrisecolour[i],10);

			skyshade[i] = Min(255,dayfade + nightfade + sunrisefade);
		}

		skyshade[3] = Min(255,progress/2);
	}
	// Day
	else if (day)
	{
		skyshade[0] = 255;
		skyshade[1] = 255;
		skyshade[2] = 255;

		skyshade[3] = 255;
	}
	// Sunset
	else if (sunset)
	{
		var time_since_sunset = time - sundown_start;
		// progress in 0..1800
		var progress = time_since_sunset * 1800 / (sundown_end - sundown_start);

		for(var i=0; i<3; ++i)
		{
			var dayfade = Cos(progress/2, daycolour[i],10);
			var nightfade = nightcolour[i] - Cos(progress/2, nightcolour[i],10);
			var sunsetfade = Sin(progress, sunsetcolour[i],10);

			skyshade[i] = Min(255,dayfade + nightfade + sunsetfade);
		}

		skyshade[3] = Min(255,900-progress/2);
	}
	// Night
	else if (night)
	{
		skyshade[0] = nightcolour[0];
		skyshade[1] = nightcolour[1];
		skyshade[2] = nightcolour[2];

		skyshade[3] = 0;
	}


	// Shade sky.
	SetSkyAdjust(RGB(skyshade[0], skyshade[1], skyshade[2]));

	// Shade landscape.
	var gamma = [0,0,0];
	var min_gamma = [30,75,120];
	gamma[0] = BoundBy(skyshade[0], min_gamma[0], 128);
	gamma[1] = BoundBy(skyshade[1], min_gamma[1], 128);
	gamma[2] = BoundBy(skyshade[2], min_gamma[2], 128);

	SetGamma(0, RGB(gamma[0], gamma[1], gamma[2]), RGB(127+gamma[0], 127+gamma[1], 127+gamma[2]), GAMMA_Ramp_DayNight);

//	if(!day && !night)
//	{
//		// Adjust celestial objects.
//		for (var celestial in FindObjects(Find_Func("IsCelestial")))
//			celestial->SetObjAlpha(255 - skyshade[3]);
//
//		// Adjust clouds
//		for(var cloud in FindObjects(Find_ID(Cloud))){
//			cloud->SetLightingShade(255 - skyshade[2]);
//		}
//	}
}

////////////////////////////////////////////////////////////////////
//
// Berechnungen und so Zeug

private func GetLightIntensity(int iTime)
{

}

////////////////////////////////////////////////////////////////////
//
// Globale Funktionen

global func GetDawnPeriod(){ return [Time(  4, 0, 0), Time(  7, 0, 0)]; }
global func GetDuskPeriod(){ return [Time( 18, 0, 0), Time( 21, 0, 0)]; }

global func Time(int hours, int minutes, int seconds)
{
	hours	= hours % 24;
	minutes	= minutes % 60;
	seconds	= seconds % 60;
	return hours * g_TIME_Hour_Seconds + minutes * g_TIME_Minute_Seconds + seconds;
}

global func GetTime()
{
	if (!time_object) return Time(12, 0, 0);
	return LocalN("time", time_object);
}

global func SetTime(int seconds)
{
	if (!time_object) return;
	LocalN("time", seconds);
}

global func GetTimeSpeed()
{
	if(!time_object) return 0;
	return LocalN("advance_seconds_per_tick", time_object);
}

global func SetTimeSpeed(int speed)
{
	if(!time_object) return 0;
	return LocalN("advance_seconds_per_tick", time_object) = speed;
}

global func IsDawn() { if(!time_object) return false; var time = GetTime(); return GetDawnPeriod()[0] <= time && time < GetDawnPeriod()[1]; }
global func IsDay()  { if(!time_object) return true;  var time = GetTime(); return GetDawnPeriod()[1] <= time && time < GetDuskPeriod()[0]; }
global func IsDusk() { if(!time_object) return false; var time = GetTime(); return GetDuskPeriod()[0] <= time && time < GetDuskPeriod()[1]; }
global func IsNight(){ if(!time_object) return false; var time = GetTime(); return GetDuskPeriod()[1] <= time || time < GetDawnPeriod()[0]; }




///* Himmelsfarbe */
//
//private func RestoreSkyColors(iPercent) {
//	// Alte Speicherung? Übertragen
//	if (Local (4)) GetOldSkyColors();
//	if (Local (6)) {
//		var i;
//		// ehemaliges OldGfx: Normales SetSkyColor
//		while(i < 20) RestoreSkyColor(i++, 100);
//	}
//	// NewGfx: Einfach SetSkyAdjust
//	// Minimale Gammakontrolle (Rampe 3)
//	var lt = iPercent / 2 + 78;
//	SetGamma(0, RGB(lt, lt, 128), 16777215, 3);
//	SetSkyAdjust(RGBa(
//		iPercent * GetRGBValue(SkyAdjustOrig,1) / 100,
//		iPercent * GetRGBValue(SkyAdjustOrig,2) / 100,
//		iPercent * GetRGBValue(SkyAdjustOrig,3) / 100,
//		iPercent * GetRGBValue(SkyAdjustOrig,0) / 100	), GetSkyAdjust(1));
//	return(1);
//}
//
//private func RestoreSkyColor(int iColor, int iPercent) {
//	SetSkyColor(iColor,
//		((Local(iColor+6)>>16 & 255) * iPercent)/100,
//		((Local(iColor+6)>> 8 & 255) * iPercent)/100,
//		((Local(iColor+6)		 & 255) * iPercent)/100
//	);
//	Local(iColor + 6) = 0;
//	return();
//}
//
//private func GetOldSkyColors() {
//	var i;
//	i=-1; while (++i<11) Local(i+ 6)=Local(i,Local(4));
//	i=-1; while (++i<11) Local(i+16)=Local(i,Local(5));
//	// Alte Hilfsobjekte entfernen
//	RemoveObject(Local(4));
//	RemoveObject(Local(5));
//	return(1);
//}
//

//}
//
//
//private func RestoreSkyColors(int iPercent)
//{
//	iPercent = 100 - iPercent;
//	//DebugLog("restoreskycolors %d", iPercent);
//	var iMin = GameCall("MinDarkness"), iMax = GameCall("MaxDarkness");
//	if(!iMax) {
//		iMax = 62; // <Nachtfalter> einigen wir uns auf 62
//	}
//	iPercent = iPercent * (iMax - iMin) / 100 + iMin;
//	//Log("fadedarkness %d", iPercent);
//	FadeDarkness(iPercent);
//	return 1;
//}
//
//
