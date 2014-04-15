#strict

/* Sets the Rotation using iX, iY as rotation center */
global func SetR2(int iR, int iX, int iY, object pObj) {
	SetR(iR, pObj);
	
	var dx = Sin(iR, iY) - Cos(iR, iX);
	var dy = -Cos(iR, iY) - Sin(iR, iX); 
	
	SetPosition(GetX(pObj)+dx+iX, GetY(pObj)+dy+iY,pObj);
}
/*
	var wdt = GetDefWidth(GetID(pObj));
	var hgt = GetDefHeight(GetID(pObj));
*/


global func GetFloorY(object pObj) {
	return GetY(pObj) + (GetCon(pObj)*GetDefHeight(GetID(pObj)))/200;
}


// From clonk infinity project - perhaps a better SetR2() ?

/** Rotiert ein Objekt um einen beliebigen Punkt herum. */
global func RelSetR(int rotation, int xPos, int yPos) {
  // Winkel zu den gedrehten Koordinaten
  var angle1 = Angle(0, 0, xPos, yPos);
  var angle2 = angle1 + GetR();
  var radius = Distance(0, 0, xPos, yPos);
  // angle2 + Radius => Umgerechneter Drehpunkt
  var rotX = (Sin(angle2, 100) * radius) / 100;
  var rotY = (-Cos(angle2, 100) * radius) / 100;
  // rotation + angle1 + Radius + umgerechneter Drehpunkt => Endpunkt
  angle1 += rotation;
  var targetX = (-Sin(angle1, 100) * radius) / 100 + rotX;
  var targetY = (Cos(angle1, 100) * radius) / 100 + rotY;
  SetPosition(GetX() + targetX, GetY() + targetY);
  SetR(rotation);
}