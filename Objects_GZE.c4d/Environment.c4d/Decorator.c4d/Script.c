/*-- Dekorator --*/

// This enviroment object is still very imcomplete

#strict 2


protected func Initialize() {
	SetPosition(0,0);
	
	//CreateObject(BRDS);
	CreateObject(CLFS);
	CreateObject(EGLN);
//	CreateObject(OWL1);
	CreateObject(STRS);
	
	
	// create Stalactites
//	PlaceVegetation2(_STA, 20, 0, LandscapeHeight() / 2, LandscapeWidth(), LandscapeHeight() / 2, Material("Granite"), 1, 0, 1);
	
	// create Underground mushrooms
	//PlaceVegetation2(MUSH, 20, 0, 0, LandscapeWidth(), LandscapeHeight(), Material("Earth"), 1);
	//Log("made shroomies");
	// create vines
//	PlaceVegetation2(VINE, 50, 0, 0, LandscapeWidth(), LandscapeHeight()/2, Material("Grass"), 0, 0, 1);
	//Log("made vines");
	// create bushes
//	PlaceVegetation2(_VEG, 50, 0, 0, LandscapeWidth(), LandscapeHeight(), Material("Gras"), 0, 0, 0);
//	Log("made bushes");
	
	//createUndergroundMushrooms();
	
	createTreasures(Max(1, LandscapeWidth() / 400));
	
	return(1);
}



func createTreasures(quantity) {
	var attempts = 5000;
	var x,y,i = 0;
	
	
	while (i++ < attempts && quantity > 0) {
		x = 50 + Random(LandscapeWidth()-100);
		y = 50 + Random(LandscapeHeight()-100);
		
		if (GetMaterial(x,y) == Material("Tunnel")) {
			var floor_y = GetYSolid(x, y);
			var chest = CreateObject(CHST, x, floor_y);
			createTreasureContents(chest, 2  + (10*floor_y) / LandscapeHeight());
			quantity--;
		}
		
	}
}

func createTreasureContents(obj, iterations) {
	while (iterations-- > 0) {
		// no switch-case :(
		var rnd = Random(19);
		if (rnd == 0) obj->CreateContents(PTRC,0,RandomX(1,3));
		if (rnd == 1) obj->CreateContents(CK5P,0,RandomX(1,3));
		if (rnd == 2) obj->CreateContents(WOOD,0,RandomX(1,4));
		if (rnd == 3) obj->CreateContents(XARP);
		if (rnd == 4) obj->CreateContents(FARP);
		if (rnd == 5) obj->CreateContents(FLNT, 0, RandomX(2,3));
		if (rnd == 6) obj->CreateContents(SFLN, 0, RandomX(1,3));
		if (rnd == 7) obj->CreateContents(CNCR,0,RandomX(3,5));
		if (rnd == 8) obj->CreateContents(METL,0,RandomX(1,3));
		if (rnd == 9) obj->CreateContents(CNKT);
		if (rnd == 10) obj->CreateContents(CRYS);
		if (rnd == 11) obj->CreateContents(LOAM, 0, RandomX(4,6));
		if (rnd == 12) obj->CreateContents(COKI, 0, RandomX(4,6));
		if (rnd == 13) obj->CreateContents(ZAPN);
		if (rnd == 14) obj->CreateContents(MUSH, 0, RandomX(1,2));
		if (rnd == 15) obj->CreateContents(BOW1);
		if (rnd == 16) obj->CreateContents(SPER);
		if (rnd == 17) obj->CreateContents(ISWR);
		if (rnd == 18) obj->CreateContents(ABRL);
	}
	if (Random(2)) {
		if (obj) {
			obj->PlaceObjects2(SKUL, Random(3), [-50, -50, 100, 100]);
			obj->PlaceObjects2(BONE, Random(6), [-50, -50, 100, 100]);
		} else {
			Log("no obj?");
		}
	}
}
/*
func createUndergroundMushrooms() {
	var attempts = LandscapeWidth() / 10;
	var x,y,i = 0;
	
	while (i++ < attempts) {
		x = Random(LandscapeWidth());
		y = Random(LandscapeHeight());
		
		if (GetMaterial(x,y) == Material("Tunnel")) {
			CreateObject(MUSH, x, GetYGround(x, y)+7);
		}
		
	}
}*/
