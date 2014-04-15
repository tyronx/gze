#strict

local playerSpawnPoints;


func Initialize() {
	playerSpawnPoints = [];
	return(1);
}


func InitializePlayer(player) {
	SetLength(playerSpawnPoints, GetPlayerID(player));
	
	var spawnpoint = getNewSpawnPoint(0);
	
	playerSpawnPoints[GetPlayerID(player)] = spawnpoint;
	
	PlacePlayer(player);
}


func getNewSpawnPoint(miny) {
	var x,y;
	var miny = (35*LandscapeHeight())/100;
	
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



func PlacePlayer(player) {
	var x = playerSpawnPoints[GetPlayerID(player)][0];
	var y = playerSpawnPoints[GetPlayerID(player)][1];
	
	SetPosition(x, y-5, GetCrew(player,0));
}


      
func RelaunchPlayer(player) {
  if (GetPlayerName(player) && !GetCrewCount(player)) {
    MakeCrewMember(Var(0)=CreateObject(KNIG,0,0,player),player);
    PlacePlayer(player);
    SetCursor(player,Var(0));
  }      
  return(1);

}

