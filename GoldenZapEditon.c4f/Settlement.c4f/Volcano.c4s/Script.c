/*-- Vulkan --*/

#strict 2

func Initialize() {
	ScriptGo(1);
	
	return(1);
}

func MusicTheme() {
	return "hell";
}

func InitializePlayer(player) {
	var x = 20 + Random(LandscapeWidth() - 40);
	
	while(FindObject2(Find_InRect(x-30, 0, 60, LandscapeHeight()), Find_Category (C4D_Structure))) {
		x = Random(LandscapeWidth());
	}
	
	// Find a goot spot
	var miny = (1*LandscapeHeight())/10;
	var spawnpoint = findSpawnPoint(miny);
	
	// Place player and hut there
	var hut = CreateConstruction(HUT2, spawnpoint[0], spawnpoint[1],player,100,1);
	SetPosition(spawnpoint[0], spawnpoint[1]-8, GetCrew(player,0));
	
	var wdt = GetDefWidth(GetID(hut)), hgt = GetDefHeight(GetID(hut));
	FreeRect(GetX(hut) - wdt / 2, GetY(hut) - hgt / 2, wdt, hgt);
	
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
