#strict 2

func Initialize() {
	MusicLevel(50);
	
	// We place some extra in-earth objects nearby certain materials for awesome looks
	PlaceObjects2(COAL, 250, [0,0,LandscapeWidth(),LandscapeHeight()], 0, Material("Coal")); // Coal nearby coal 
	PlaceObjects2(FLNT, 80, [0,0,LandscapeWidth(),LandscapeHeight()], 0, Material("Flint"));
	PlaceObjects2(LOAM, 100, [0,0,LandscapeWidth(),LandscapeHeight()], 0, Material("Loam"));
	PlaceObjects2(ORE1, 150, [0,0,LandscapeWidth(),LandscapeHeight()], 0, Material("Ore"));

	return(1);
}


func InitializePlayer(player) {
	var x = 20 + Random(LandscapeWidth() - 40);
	
	while(FindObject2(Find_InRect(x-30, 0, 60, LandscapeHeight()), Find_Category (C4D_Structure))) {
		x = Random(LandscapeWidth());
	}
	
	// Find a goot spot
	var miny = (4*LandscapeHeight())/10;
	var spawnpoint = findSpawnPoint(miny);
	
	// Place player and hut there
	var hut = CreateConstruction(HUT2, spawnpoint[0], spawnpoint[1],player,100,1);
	SetPosition(spawnpoint[0] - 5, spawnpoint[1]-8, GetCrew(player,0));
	
	// Amboss drüberklatschen
	hut->CreateObject(ANVL, 17, 23);
	hut->CreateContents(FLAG);
}



func findSpawnPoint(miny) {
	var x,y;
	
	while (true) {
		x = 20 + Random(LandscapeWidth() - 40);
		y = miny;
		
		while (GBackSolid(x, y) && y < LandscapeHeight()) {
			y++;
		}
		y = GetYSolid(x, y);
		
		if (LandscapeHeight() - y < 10) continue;
		if (FindObject2(Find_InRect(x-30, y - 80, 60, 160), Find_Category (C4D_Structure))) continue;
		if (GBackLiquid(x, y-3)) continue;
		
		return [x,y];
	}
	return [];
}
