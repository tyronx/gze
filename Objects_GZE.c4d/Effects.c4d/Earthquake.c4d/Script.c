/* Erdbeben */

#strict 2

local iLevel;
local iLifeTime;

func ShakeEarth() {
	
	if(!Random(Max(2, iLevel / 50))) return(1);

	ShakeObjects(GetX(),GetY(),iLevel);

	var x = GetX()+Random(2*iLevel+1)-iLevel;
	var y = GetY()+Random(2*iLevel+1)-iLevel;
	var step_x = Random(7)-3;
	var step_y = Random(5)-2;
	var i, amount = 35+Random(6);
	
	while (++i<amount) {
		ShakeFree(
			x+=step_x,
			y+=step_y,
			Random(15)
		);
	}
  
	if(GetActTime()>iLifeTime) return(RemoveObject());	
}

// LaunchEarthquake(GetX(),GetY())
global func LaunchEarthquake(int iX, int iY, int iLevel) {
	var quake=CreateObject(FXQ1,iX-GetX(),iY-GetY(),-1);
	quake->Activate(iLevel);
	return(quake);
}
	
public func Activate(int _iLevel) {
	iLevel=_iLevel;
	if(!iLevel) iLevel=100+Random(101);
	iLifeTime=iLevel/2;
	SetAction("Quake");
	var eff=AddEffect("QuakeEffect",this(),200,1,this());
	EffectVar(0,this(),eff)=iLevel;
	// Sch�tteleffekt am Anfang und Ende schw�cher (Berechnung der Konstanten der Funktion)
	// iStrength(iTime) = a*x - b*x^2 (verlauf einer parabel wimni)
	// Maximalwert: iMaxStrength = iLevel/10
	// Errechnung: a = 4 * iMaxStrength / iTime
	//                       b = a / iTime
	EffectVar(1,this(),eff) = (4*10*iLevel) / (10*iLifeTime);
	EffectVar(2,this(),eff) = (100*EffectVar(1,this(),eff)) / iLifeTime;
	
	return(1);
}

func FxQuakeEffectTimer(object pTarget, int iEffectNumber, int iTime) {
	var iPlr, iLevel;
	var a=EffectVar(1,pTarget,iEffectNumber), b=EffectVar(2,pTarget,iEffectNumber);
	for(var i=0; i<GetPlayerCount(); i++) {
		iPlr=GetPlayerByIndex(i);
		// Sch�tteleffekt ist am Anfang und Ende des Erdbebens schw�cher
		iLevel=(a*iTime-(b*iTime**2)/100)/10;
		
		// Sch�tteleffekt verringert sich je nach Entfernung, max. St�rke 50
		iLevel=Min(50, (300*iLevel)/Max(300,Distance(GetX(GetCursor(iPlr)),GetY(GetCursor(iPlr)),GetX(),GetY())));
		SetViewOffset(iPlr, Sin(iTime*100,iLevel), 0); // Cos(iTime*100,iLevel));
	}
	
	if((a*iTime-(b*iTime**2)/100)/10<0) {
			return(-1);
	}
}