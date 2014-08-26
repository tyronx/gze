/* Vehicle Transporter Lib */

#strict 2

// Needs to be relative Coordinates x,y,wdt,hgt  
func VehicleGrabArea() { return [-15, -10, 30, 20]; }
func VehicleHoldingArea() { return [-8,-10,16,30]; }
func IsVehicleTransporter() { return 1; }

// Example func to determine when to relay clonk controls to the held vehicle
func CanRelayControl(controltype, pClonk) { 
	if (controltype == "ControlLeft" || controltype == "ControlRight") {
		return GetAction() == "Wait";
	}
	return 1; 
}


func Initialize() {
	AddEffect("Activity", this, 1, 1, this);
}

func FxActivityTimer() {
	GrabVehicles();
	ChangeClonkGrabs();
}



func GrabVehicles() {
	var grabarea = VehicleGrabArea(), holdarea = VehicleHoldingArea();
	// Find Vehicles in the Grab Area but outside the holding area, push these into the holding area
	for (var pVehicle in FindObjects(
		Find_InRect(grabarea[0], grabarea[1], grabarea[2], grabarea[3]),
		Find_Not(Find_InRect(holdarea[0], holdarea[1], holdarea[2], holdarea[3])),
		Find_Category(C4D_Vehicle),
		Find_OCF(OCF_Grab),
		Find_NoContainer(),
	)) {
	
		if (FitsInTransporter(pVehicle)
			&& Inside(GetXDir(pVehicle, 100), -1, +1)
			&& !pVehicle->~IsVehicleTransporter()
			&& GetProcedure(pVehicle) != "FLOAT"
		)  {
	  
			GrabVehicle(pVehicle);
	}
  }
}

func FitsInTransporter(pObject) {
	// Alle (ungedrehten) Vertices des Objekts prüfen
	var vertexCount = pObject->GetVertexNum();
	var holdingarea = VehicleHoldingArea();
	var holdingareasize = [holdingarea[2] - holdingarea[0], holdingarea[3] - holdingarea[1]];
	
	for (var i = 0; i < vertexCount; i++)  {
		var vertexX = pObject->GetVertex(i, 0); 
		var vertexY = pObject->GetVertex(i, 1);
		// Passt nicht
		if (!Inside(vertexX, -holdingareasize[0]/2, holdingareasize[0]/2) || !Inside(vertexY, -holdingareasize[1]/2, holdingareasize[1]/2)) return(0);
	}
	// Passt
	return(1);  
}


func GrabVehicle(obj) {
	var holdingarea = VehicleHoldingArea();
	var holdingareabounds = [GetX() + holdingarea[0], GetY() + holdingarea[1], GetX() + holdingarea[0] + holdingarea[2], GetY() + holdingarea[1] + holdingarea[3]];
	
	var x = GetX(); /*BoundBy(
		GetX(obj) - GetX(), 
		// todo: replace GetObjWidth() with outermost vertex positions
		holdingareabounds[0] + GetObjWidth(obj)/2,		// most left possible
		holdingareabounds[2] - GetObjWidth(obj)/2			// most right possible
	);*/
	var y = GetY() + (GetObjHeight(this()) - GetObjHeight(obj)) / 2 - 1;
	
	obj->SetSpeed(0, GetYDir() / 20); 
	obj->SetRDir(0);
	obj->SetR(0);
	obj->SetPosition(x, y);
}


func ChangeClonkGrabs() {
	var clonk, area = VehicleHoldingArea();
	
	for (var pClonk in FindObjects(Find_InRect(area[0], area[1], area[2], area[3]), Find_OCF(OCF_Living | OCF_NotContained), Find_Action("Push"))) {
		var target = GetActionTarget(0, pClonk);
		
		// Do we got a clonk nearby, grabbing a transportable vehicle?
		if (IsClonkIdle(pClonk) && target && !target->~IsVehicleTransporter()) {
			
			// Already grabbing but far out, put Clonk closer to middle
			if (target == this && Abs(GetX(pClonk) - GetX()) > 8) {
				SetCommand(pClonk, "UnGrab");
				AppendCommand(pClonk, "MoveTo", 0, BoundBy(clonk->GetX(), GetX()-5, GetX()+5), GetY());
				AppendCommand(pClonk, "Grab", this);
				continue;
			}
			
			// Not yet grabbing and Close enough => Grab Elev and Center the Vehicle-To-Be-Transported
			if (IsVehicleNearby(target)) {
				GrabVehicle(target);
				SetCommand(pClonk, "Grab", this());
			}
		}
	}
}


func IsClonkIdle(pClonk) {
	return 
		(GetComDir(pClonk) == COMD_Stop || GetComDir(pClonk) == COMD_Up)
		&& Abs(GetXDir(pClonk)) < 3
		&& (!GetCommand(pClonk) || GetCommand(pClonk) == "None")
		&& !GetMenu(pClonk);
}

func IsVehicleNearby(pVehicle) {
	return
		IsInHoldingArea(pVehicle)
		&& Inside(GetXDir(pVehicle), -2, +2)
		&& FitsInTransporter(pVehicle);
}

// Also checks if the path between middle of obj to the middle of the grabbing area is free
func IsInArea(pObj, rect) {
	return 
		Inside(GetX(pObj)-GetX(), rect[0], rect[0] + rect[2])
		&& Inside(GetY(pObj)-GetY(), rect[1], rect[1] + rect[3])
		&& PathFree(GetX() + rect[0] + rect[2]/2, GetY() + rect[1] + rect[3] / 2, GetX(pObj), GetY(pObj));
}
func IsInHoldingArea(pObj) {
	return IsInArea(pObj, VehicleHoldingArea());
}
func IsInGrabArea(pObj) {
	return IsInArea(pObj, VehicleGrabArea());
}
func IsOnlyInGrabArea(pObj) {
	return 
		IsInArea(pObj, VehicleGrabArea())
		&& !IsInHoldingArea(pObj);
}


func ControlLeft(pObj) { return Control("ControlLeft", pObj); }
func ControlRight(pObj) { return Control("ControlRight", pObj); }
func ControlUp(pObj) { return Control("ControlUp", pObj); }
func ControlDown(pObj) { return Control("ControlDown", pObj); }
func ControlDig(pObj) { return Control("ControlDig", pObj); }
func ControlDigDouble(pObj) { return Control("ControlDigDouble", pObj); }
func ControlThrow(pObj) { return Control("ControlThrow", pObj); }
func ControlDownSingle(pObj) { return Control("ControlDownSingle", pObj); }


func Control(type, pObj) {
	if (CanRelayControl(type, pObj)) {
		return RelayControl(type, pObj);
	}
	return 0;
}


func HeldVehicles(vehicle) {
	var area = VehicleHoldingArea();
	return FindObjects(
		Find_InRect(area[0], area[1], area[2], area[3]), 
		Find_OCF(OCF_Grab), 
		Find_NoContainer(), 
		Find_Category(C4D_Vehicle),
		Find_Not(Find_ID(GetID()))
	);
}

func RelayControl(controltype, pClonk, silent) {
	// Gets first Vehicle
	var vehicles = HeldVehicles();
	
	if (!vehicles) {
		if (!silent) Sound("Click");
		return 0;
	}
	
	var vehicle = ArrayShift(vehicles);
	
	if (controltype == "ControlThrow")  {
		var getput = GetDefGrabPutGet(GetID(vehicle));
		// Kommando per Script überladen?
		if (PrivateCall(vehicle, controltype, pClonk)) return true;
		
		// Will etwas aus dem Fahrzeug rausnehmen?
		if (ContentsCount(0, pClonk) == 0 && getput & 2)  {
			SetCommand(pClonk, "Get", vehicle, 0,0, 0, 1);
		} else {
			// Will etwas ins Fahrzeug reinlegen
			if (getput & 1)  {
				SetCommand(pClonk, "Put", vehicle);
				AppendCommand(pClonk, "Grab", this());
			}
		}

		return 1;
	}

	if (!PrivateCall(vehicle, controltype, pClonk))  {
		var comd;
		if (controltype == "ControlLeft") comd = COMD_Left;
		if (controltype == "ControlRight") comd = COMD_Right;
		if (comd) SetComDir(comd, pClonk);
	}
	
	// Change grab target of the clonk
	SetActionTargets(vehicle, 0, pClonk);
	
	return 1;
}