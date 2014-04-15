#strict 2

local statiByPlayer;
local maxPlayerIndex;

local names, enviromentconditions, sounds, playchances, volumes, loops;

// Local variable usage:

// Local(0..highest playerid)   - Local index coresponds to player id
// = Array ( 
//		0 = Array([amount of env conditions numbers: between 0 and 3, whereas 0 means stop that sound and 3 means play that sound
//		1 = Array([amount of env conditions numbers: holds object where sound is played at])
//	)


protected func Initialize() {
	SetPosition(0,0);
	names = [];
	enviromentconditions = [];
	sounds = [];
	playchances = [];
	volumes = [];
	loops = [];
	
	//AddAmbient("wind", ["inMat_Sky"], "RealWind", 100, "script_Abs(GetWind()/10)*10", 1);
	AddAmbient("caves", ["inMat_Tunnel"], "Cave*", 5);
	AddAmbient("cavesandwater", ["inMat_Tunnel", "nearbyMat_Water"], "WaterCave", 100);
	AddAmbient("highwinds", ["inMat_Sky", "maxPosY_10"], "HighWinds", 100);
	AddAmbient("lavabubble", ["nearbyMat_DuroLava"], "LavaBubble", 100);
	AddAmbient("beaches", ["nearbyMat_Water", "nearbyMat_Sand", "inMat_Sky"], "ShoreWaves", 100);
	AddAmbient("nightowlsintrees", ["nearbyDefcall_IsTree", "IsNight_1"], "Owl", 10);
	AddAmbient("daybirdsintrees", ["nearbyDefcall_IsTree", "IsNight_0"], "Bird*", 20);
	AddAmbient("nightcicadasingrass", ["nearbyId_GRAS", "IsNight_1"], "Cicada*", 20);
	AddAmbient("underwater", ["inMat_Water"], "UnderWater", 100);

	AddEffect("Ambient", this, 1, 40, this);
	
	return(1);
}

func AddAmbient(string name, array conditions, string sound, int playchance, string volume, int loop) {
	ArrayPush(names, name);
	ArrayPush(enviromentconditions, conditions);
	ArrayPush(sounds, sound);
	ArrayPush(playchances, playchance);
	
	ArrayPush(volumes, volume);
	ArrayPush(loops, loop);
}

func RemoveAmbient(string name) {
	var index = ArrayIndexOf(names, name);
	if (index > -1) {
		ArraySlice(names, index);
		ArraySlice(enviromentconditions, index);
		ArraySlice(sounds, index);
		ArraySlice(playchances, index);
		
		ArraySlice(volumes, index);
		ArraySlice(loops, index);
	}
}


func FxAmbientTimer() {
	//Log("ambient check");
	checkPlayerCount();
	updatePlayerStati();
	playAmbient();
}

func checkPlayerCount() {
	var playerid;
	for (var i = 0; i < GetPlayerCount(C4PT_User); i++) {
		playerid= GetPlayerByIndex(i, C4PT_User);
		if (!Local(playerid)) {
			Local(playerid) = CreateArray(2);
			Local(playerid)[0] = CreateArray(GetLength(enviromentconditions));
			Local(playerid)[1] = CreateArray(GetLength(enviromentconditions));
		}
	}
}

func updatePlayerStati() {
	var clonk, plr=0, playerid, envnum, found = 0;
	
	var type, target;
	
	while (clonk = GetCursor(plr++)) {
		playerid = GetPlayerByIndex(GetOwner(clonk));
		//Log("%v", enviromentconditions);
		
		for (envnum = 0; envnum < GetLength(enviromentconditions) - 1; envnum++) {
			//Log("%d %d", envnum, GetLength(enviromentconditions));
			found = 1;
			//Log("envnum %d", envnum);
			for (var prop in enviromentconditions[envnum]) {
				var tmp = ExplodeString("_", prop);
				type = tmp[0];
				target = tmp[1];
				
				if (type S= "IsNight") {
					found = found && Format("%d", IsNight()) S= target;
				}
				
				if (type S= "minPosY") {
					found = found && clonk->GetY() >= (StringToInt(target) * LandscapeHeight()) / 100;
				}
				if (type S= "maxPosY") {
					found = found && clonk->GetY() <= (StringToInt(target) * LandscapeHeight()) / 100;
				}
				if (type S= "minPosX") {
					found = found && clonk->GetX() >= (StringToInt(target) * LandscapeWidth()) / 100;
				}
				if (type S= "maxPosX") {
					found = found && clonk->GetX() <= (StringToInt(target) * LandscapeWidth()) / 100;
				}
				
				if (type S= "inMat") {
					//Log("%s %s", MaterialName(GetMaterial(clonk->GetX(), clonk->GetY())), target);
					found = found && GetMaterial(clonk->GetX(), clonk->GetY()) == Material(target);
				}
				
				if (type S= "nearbyMat") {
					var haveMat = 0;
					for (var attempt = 0; attempt < 80; attempt++) {
						if (GetMaterial(clonk->GetX()+RandomX(-100, 100), clonk->GetY()+RandomX(-100, 100)) == Material(target)) {
							haveMat = 1;
							break;
						}
					}
					found = found && haveMat;
				}
				
				if (type S= "nearbyDefcall") {
					var haveCall = 0;
					for (var obj in FindObjects(Find_Distance(30, clonk->GetX(), clonk->GetY()))) {
						if (DefinitionCall(GetID(obj), target)) {
							haveCall = 1;
							break;
						}
					}
					found = found && haveCall;
				}
				
				if (type S= "nearbyId") {
					found = found && FindObject2(Find_ID(C4Id(target)), Find_Distance(30, clonk->GetX(), clonk->GetY()));
				}
				
				
			}
			
			updatePlayerProp(playerid, envnum, found);
		}
	}
}

func updatePlayerProp(playerid, envnum, increase) {
	if (increase) {
		Local(playerid)[0][envnum] = Min(3, Local(playerid)[0][envnum]+1);
	} else {
		Local(playerid)[0][envnum] = Max(0, Local(playerid)[0][envnum]-1);
	}
}


private func playAmbient() {
	var lastplayerid = GetPlayerByIndex(GetPlayerCount(C4PT_User)-1, C4PT_User);
	var plrnum = 0;
	//Log("%v", Local(plrid));
	
	for (var plrid = 0; plrid <= lastplayerid; plrid++) {
		if (Local(plrid)) {
			for (var amb=0; amb <= GetLength(names); amb++) {
				if (Local(plrid)[0][amb] >= 3) {
					playSound(sounds[amb], amb, plrid, plrnum, volumes[amb], loops[amb]);
				}
				if (Local(plrid)[0][amb] <= 0) {
					stopSound(sounds[amb], amb, plrid, plrnum);
				}
			}
			plrnum++;
		}
	}
	
	/*if (IsNight()) {
		if (!Random(20)) Sound("Cicada*",1);
		if (!Random(30)) Sound("Owl", 1);
	} else {
		if (!Random(8)) Sound("BirdSong*",1);
	}*/
}

private func playSound(sound, ambid, plrid, plrnum, volume, loop) {
	if (loop && Local(plrid)[1][ambid]) {
		if (Local(plrid)[1][ambid]->Local(0) != volume) {
			stopSound(sound, ambid, plrid, plrnum); 
		}
	}
	
	if (!Local(plrid)[1][ambid]) {
		//Log("sound %v started for player %d", sounds[ambid], plrnum+1);
		Local(plrid)[1][ambid] = CreateObject(AMBH, 0, 0, -1);
		Local(plrid)[1][ambid]->Local(0) = volume;
	}
	
	
	if (GetType(volume) == C4V_String && IndexOf("script_", volume) != -1) {
		volume = eval(Substring(volume, 7));
	}
	
	
	Log("sound %v with vol %v (ambid %d, plrid %d, plrnum %d)", sound, volume, ambid, plrid, plrnum);
	
	if (Random(100) < playchances[ambid]) {
		Local(plrid)[1][ambid]->Sound(sounds[ambid], true, 0, volume, plrnum+1, loop);
	}
}

//Sound("WaterCave", true, this(), 100, 1, 0, -1)
//Sound("WaterCave", true, this(), 100, 1, 0, -1)
private func stopSound(sound, ambid, plrid, plrnum) {
	if (Local(plrid)[1][ambid]) {
		//Log("sound %v stopped for player %d", sounds[ambid], plrnum+1);
		Local(plrid)[1][ambid]->Sound(sounds[ambid], true, 0, 100, plrnum+1, -1);
		RemoveObject(Local(plrid)[1][ambid]);
	}
}

