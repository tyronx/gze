#strict 2

local quantityrules;

func Initialize() {
	quantityrules++;
	
	var otherrule;
	if (otherrule=FindObject(PEVO)) {
		otherrule->LocalN("quantityrules")++;
		RemoveObject();
	} else {
		ScheduleCall(this(), "LateInitialize", 1);
	}
}

func LateInitialize() {
	SetPosition(0,0);
	PlaceVolcanoes();
	RemoveObject();
}


func PlaceVolcanoes() {
	var quantity = quantityrules;
	var attempts = 10000;
	var x,y, validspot;
	
	while (quantity > 0 && attempts-- > 0) {
		x = 50 + Random(LandscapeWidth()-100);
		y = 50 + Random(LandscapeHeight()-100);
		
		// Needs to be ontop of a lava lake
		validspot = false;
		
		while (GetMaterial(x,y) == Material("DuroLava") && y > 20) {
			y--;
			if (GetMaterial(x, y) == Material("Tunnel") || GetMaterial(x, y) == 0) {
				validspot = true;
				break;
			}
		}
		
		
		// Shall not be at the border of a lake
		validspot = validspot && GetMaterial(x+12,y+1) == Material("DuroLava") && GetMaterial(x-12,y+1) == Material("DuroLava");
		
		// Shall not be near another lava eruption
		validspot = validspot && !FindObject2(Find_ID(FXVP), Find_Distance(150, x, y));
		
		
		if (validspot) {
			var volcano = CreateObject(FXVP, x, y);
			var strength = RandomX(10+7*quantityrules,50+7*quantityrules);
			
			if (GameCall("PermaVolcanoLargeEruptions")) strength = RandomX(80,130);

			volcano->Activate(strength, Material("DuroLava"));
			quantity--;
		}
	}
}