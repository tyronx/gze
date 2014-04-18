/* Vulkan */
#strict 2

local iWidth,iMat, iXDir;
local OldX, OldY;

local eruptTime; 

/* Ausbruch */ 

func Advance() 
{
	var advancex=Random(11)-5+iXDir;
	var advancey=Random(8)-12;
	var obj, xdir, ydir;
	
	if(!InLava()) 
	{
		return(SetAction("Discharge"));
		xdir=30-Random(60);
		ydir=-20-Random(50)-iWidth/2;
	} else {
		xdir=(GetX()-OldX)*10;
		ydir=(GetY()-OldY)*10;
	}
	// Objekte mitschleudern
	while(obj=FindObject(0,Max(OldX,GetX())-Min(OldX,GetX())-iWidth/2,0,iWidth+Abs(OldX-GetX()),OldY-GetY(),OCF_Collectible,0,0,0,obj)) 
	{
		SetYDir(ydir, obj);
		SetXDir(xdir, obj);
	}
	OldX=GetX();
	OldY=GetY();
	
	SetPosition(GetX()+advancex,GetY()+advancey);
	
	if(iWidth>5) 
	{
		if(!Random(3)) Sound("Lava*");
	}

	if (iWidth>10)
	{
		if(!Random(10)) ShakeViewPort(10+Random(iWidth),this);
	}
	
	DrawMaterialQuad(
		Format("%s-Flat",MaterialName(iMat)),
		GetX()-iWidth/2, 
		GetY()+4,
		GetX()+iWidth/2, 
		GetY()+4,
		OldX+iWidth/2,
		OldY,
		OldX-iWidth/2,
		OldY,
		1
	);
}


/* Eruption */ 
func CastLava() 
{
	// Perma Vulkane halten den gleichen Flüssigkeitslevel
	if (InLava()) 
	{
		if (GameCall("PermaVolcanoRisingLava")) 
		{
			return(SetAction("Advance"));
		} else {
			for(var i; i<40; i++) ExtractLiquid(0,2);
		}
	}
	
	// dicke Rauchschwaden
	if (eruptTime > 0) {
		for (var i = 0; i <= 2 + iWidth / 4; i++) {
			Smoke(-iWidth / 2 + Random(iWidth), +15, 20 + Random(5 + iWidth / 2), HSL(38, 200, 50)); 
		}

	} else {
		if (Random(2)) {
			Smoke(-iWidth / 2 + Random(iWidth), +15, 10 + Random(iWidth / 2), HSL(38, 180, 200)); 
		}
	}
	
    // rumspritzen
	if (eruptTime > 0) {
		var y_diff;
		while(GetMaterial(0,y_diff)==iMat) y_diff--;
	
		//CastPXSX(string mat,int am,int lev,int x,int y,int angs,int angw) {
		CastPXSX(MaterialName(iMat), Max(iWidth/4,2), Max(40, iWidth) + Random(50), -iWidth/2 + Random(iWidth), y_diff - 1, 270 + 10 - Random(21), 20 + Random(30));
		// mit Lavaklumpen um sich werfen
		if(MaterialName(iMat) =="Lava" || MaterialName(iMat) =="DuroLava") {
			if(iWidth>10) {
				if(!Random(20)) {
					CastObjectsX(LAVA,1,Max(40,iWidth)+Random(30),-iWidth/2+Random(iWidth),0,270,50,"LavaCast");
					Sound("Discharge");
					if (!Random(50)) ShakeViewPort(10+Random(20)+Random(iWidth),this());
			  }
			}
		}
		
		eruptTime--;
	} else {
		if (!Random(2000)) eruptTime = RandomX(150, 1000);
	}
}

/* Callback of CastObjectsX */
func LavaCast(obj) {
	obj->SetMaterial(MaterialName(iMat));
	DoCon(-50+Random(5*iWidth)+iWidth/2,obj);
}

/* Aktivierung des Vulkans */
func Activate(iStr,iM) {
	if(!(iMat=GameCall("VolcanoMaterial"))) iMat=iM;
	iWidth=iStr;
	
	SetAction("Discharge");
	if(!Random(5)) Sound(Format("%s*",MaterialName(iMat)));
	return(1);
}

/* Prüfungen */

private func InLava() {
   if (GetMaterial(0,2) == iMat || (iMat==Material("Lava") && GetMaterial(0,2)==Material("DuroLava"))) return(1);
  return(0);
}

public func GetVegetationRootDepth() { return(-1); }