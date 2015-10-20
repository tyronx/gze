#strict 2

protected func Initialize() {
	SetPosition(0,0);
	createTreasures(Max(1, LandscapeWidth() / 400));
	return(1);
}



func createTreasures(quantity) {
	var attempts = 5000;
	var x,y,i = 0;
	
	var placed = 0;
	
	while (i++ < attempts && quantity > 0) {
		x = 50 + Random(LandscapeWidth()-100);
		y = 50 + Random(LandscapeHeight()-100);
		
		if (GetMaterial(x,y) == Material("Tunnel")) {
			var floor_y = GetYSolid(x, y);
			var chest = CreateObject(CHST, x, floor_y);
			createTreasureContents(chest, 2  + (10*floor_y) / LandscapeHeight());
			quantity--;
			placed++;
		}
	}
	
	Log("Placed %d Treasures", placed);
}

func createTreasureContents(obj, iterations) {
	while (iterations-- > 0) {
		// Meh, no switch-case :(
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