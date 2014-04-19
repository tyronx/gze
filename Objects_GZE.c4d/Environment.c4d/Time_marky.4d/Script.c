/*-- Tageszeiten --*/

#strict

/* Locals */

//	 0: Geschwindigkeit 1-10
//	 1: Tageszeit 0-TimeResolution (Mittag-Mittag)
//	 2: Helligkeit 0-100
// 4,5: Hilfsobjekte (Abwärtskompatiblität)
// 6-25: Himmelsfarben (Abwärtskompatiblität)

local SkyAdjustOrig;

private func TimeResolution() { return(10000); }
private func DayTimeLength() { return(TimeResolution() / 2); }

/* Verzögerte Initialisierung */

protected func Initialize() {
	if(!ObjectCount2(Find_ID(DARK))) {
		CreateObject(DARK, 0, 0, NO_OWNER);
	}
	SetDarkness(GameCall("MinDarkness"));

	SetAction("Initialize");
	// Starthelligkeit
	Local(2) = 100;
	return(1);
}

private func Initialized() {
	// Geschwindigkeit ermitteln
	Local() = ObjectCount(GetID()) + 1;
	// Andere Objekte des gleichen Typs entfernen
	var pOther;
	while(pOther = FindObject(GetID()))
		RemoveObject(pOther);
	// Himmelsmodulation speichern
	SkyAdjustOrig = GetSkyAdjust();
	// Steuerungsaktion
	SetAction("Time");
	// Fertig
	return(1);	
}
	
/* Helligkeitsberechnung */
private func GetLightIntensity(iTime) {
	return BoundBy(Cos((iTime * 360) / TimeResolution(), 250) - Cos((DayTimeLength() * 180) / TimeResolution(), 300), -50, 50) + 50;
}
	
/* Konfiguration */

public func SetTime(iTime) { // 0 bis 100 = Mittag bis Mittag
	UpdateTimeInfo( false );
	// Zeit setzen
	Local(1) = iTime * TimeResolution() / 100;
	// Helligkeit anpassen
	RestoreSkyColors(Local(2) = GetLightIntensity(Local(1)));
	return(1);
}
	
/* Zeitverlauf */	

private func Timing() {
	UpdateTimeInfo( true );
	// Zeitfortschritt
	Local(1) = (Local(1) + Local(0)) % TimeResolution();
	// Helligkeit nach Tageszeit berechnen
	var iIntensity = GetLightIntensity(Local(1));
	// Helligkeit und Himmelsfarbe anpassen
	if (iIntensity > Local(2)) RestoreSkyColors(++Local(2));
	if (iIntensity < Local(2)) RestoreSkyColors(--Local(2));
	// Fertig
	return(1);
}
	
protected func UpdateTransferZone() {
	// Kleiner Trick, damit der Himmel nach dem Laden aktualisiert wird
	if (GetAction() S= "Time") 
		RestoreSkyColors(Local(2));
	return(0);	
}
	
/* Status */

global func IsDay() {
	var pTime; 
	if (!(pTime = FindObject(TIME))) return(1);
	if (Local(2, pTime)) return(1);
	return(0);
}
	
global func IsNight()
{
	return(!IsDay());
}
		
/* Himmelsfarbe */

private func RestoreSkyColors(iPercent) {
	// Alte Speicherung? Übertragen
	if (Local (4)) GetOldSkyColors();
	if (Local (6)) {
		var i;
		// ehemaliges OldGfx: Normales SetSkyColor
		while(i < 20) RestoreSkyColor(i++, 100);
	}
	// NewGfx: Einfach SetSkyAdjust
	// Minimale Gammakontrolle (Rampe 3)
	var lt = iPercent / 2 + 78;
	SetGamma(0, RGB(lt, lt, 128), 16777215, 3);
	SetSkyAdjust(RGBa(
		iPercent * GetRGBValue(SkyAdjustOrig,1) / 100,
		iPercent * GetRGBValue(SkyAdjustOrig,2) / 100,
		iPercent * GetRGBValue(SkyAdjustOrig,3) / 100,
		iPercent * GetRGBValue(SkyAdjustOrig,0) / 100	), GetSkyAdjust(1));
	return(1);
}
	
private func RestoreSkyColor(int iColor, int iPercent) {
	SetSkyColor(iColor,
		((Local(iColor+6)>>16 & 255) * iPercent)/100,
		((Local(iColor+6)>> 8 & 255) * iPercent)/100,
		((Local(iColor+6)		 & 255) * iPercent)/100
	);
	Local(iColor + 6) = 0;
	return();
}

private func GetOldSkyColors() {
	var i;
	i=-1; while (++i<11) Local(i+ 6)=Local(i,Local(4));
	i=-1; while (++i<11) Local(i+16)=Local(i,Local(5));
	// Alte Hilfsobjekte entfernen
	RemoveObject(Local(4));
	RemoveObject(Local(5));
	return(1);
}







static time_years,time_days,time_hours,time_minutes;
static time_hours_old;

static const gYearLength = 20; // so viele Tage dauert ein Jahr

static const gTime_FramesPerSecond = 35;
static const gTime_FramesPerMinute = 2100;

static const gTime_Morning = 5;
static const gTime_Evening = 18; // es wird ab ca. 19 Uhr dunkel

static const gTime_Compensation = false;

static const gTime_Oldschool = false;

global func TimeResolution() { return 12000; } // macht das Rechnen leichter; war 10000

global func TimeSpeed() {
	var time = FindObject(TIME);
	if(!time) return 1;

	return Local(0,time);
}

global func GetTimePoints(&evening, &morning) {
	if(gTime_Oldschool) {
		evening = TimeResolution() / 4;
		morning = 3 * TimeResolution() / 4;
	} else {
		evening = ((11 + gTime_Evening)%24) * TimeResolution() / 24; // hier beschiss um 1 Einheit ^^
		morning = ((10 + gTime_Morning)%24) * TimeResolution() / 24;
	}
	var pObj = FindObject2(Find_Category(C4D_Rule), Find_Func("OverloadsTime"));
	if(pObj) pObj ->~TimeOverload(evening, morning);
	return 1;
}

private func GetLightIntensity(int iTime) {
	var evening, morning;
	GetTimePoints(evening, morning);
	
	var fade = TimeResolution() / 100;
	evening -= fade / 2;
	morning -= fade / 2;
	
	// ganz hell?
	if(iTime < evening || iTime > morning + fade)
		return 100;
	
	if(iTime - evening > fade) {
		iTime -= morning;
	} else {
		iTime -= evening;
	}
	
	var intensity = Max(0, iTime * 100 / fade);

	//DebugLog("getlightintensity: %d", intensity);
	return intensity;
} 


private func RestoreSkyColors(int iPercent) {
	iPercent = 100 - iPercent;
	//DebugLog("restoreskycolors %d", iPercent);
	var iMin = GameCall("MinDarkness"), iMax = GameCall("MaxDarkness");
	if(!iMax) {
		iMax = 62; // <Nachtfalter> einigen wir uns auf 62
	}
	iPercent = iPercent * (iMax - iMin) / 100 + iMin;
	//Log("fadedarkness %d", iPercent);
	FadeDarkness(iPercent);
	return 1;
}

private func UpdateTimeInfo( bool bCountDays ) {
	if( bCountDays && Local(1) <= Local(0) ) time_days++;

	if(time_days >= gYearLength)
	{
		time_days = 0;
		time_years++;
	}

	var hours = Local(1) - TimeResolution()/2;
	if( hours < 0 ) hours+=TimeResolution();
	hours*=2;

	var minutes = hours % ( TimeResolution()/12 );

	time_hours = (12*hours)/TimeResolution();

	// Zeit-Ausgleich
	if(gTime_Compensation) time_hours -= 2;

	if( time_hours < 0 ) time_hours += 24;
	if( time_hours > 24 ) time_hours -= 24;

	if( time_hours != time_hours_old )
	{
		var obj;
		for(obj in FindObjects(Find_Func("UpdateTime",time_hours, time_hours_old)));

		time_hours_old = time_hours;
	}

	//time_minutes = 60*minutes()/( TimeResolution()/12 );
	time_minutes = 720*minutes/TimeResolution();
}

global func TimeSetHour( int iHour ) {
	var pTime = FindObject(TIME);

	if(!pTime) return;

	// Zeit-Ausgleich
	// mit dem Darkness-Objekt geht die Nacht von 20 Uhr bis 9 Uhr
	// mit dem Ausgleich geht die Nacht von 18 Uhr bis 7 Uhr
	if(gTime_Compensation) iHour = (iHour+3)%24;

	var x = (100*(iHour+12)/24)%100;
	pTime->SetTime( x );

	return x;
}


global func IsDawn() { if(FrameCounter() < 10) return false; return time_hours == gTime_Morning; }
global func IsDay() { if(FrameCounter() < 10) return true; return time_hours > 6 && time_hours < 19; } // 7-18 Uhr
global func IsDusk() { if(FrameCounter() < 10) return false; return time_hours == gTime_Evening; } // 18-20 Uhr

global func TimeGetSpeed() {
	var time = FindObject(TIME);
	if(!time) return -1;

	return Local(0,time);
}

global func TimeSetSpeed(int speed) {
	var time = FindObject(TIME);
	if(!time) return 0;

	return Local(0,time) = speed;
}

