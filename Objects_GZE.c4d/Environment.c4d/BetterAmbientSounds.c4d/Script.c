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
	
	AddAmbient("wind", ["inMat_Sky"], "RealWind", 100, "script_Abs(GetWind()/10)*10", 1);
	AddAmbient("caves", ["inMat_Tunnel"], "Cave*", 5);
	AddAmbient("cavesandwater", ["inMat_Tunnel", "nearbyMat_Water"], "WaterCave", 100);
	AddAmbient("highwinds", ["inMat_Sky", "maxPosY_10"], "HighWinds", 100);
	AddAmbient("lavabubble", ["nearbyMat_DuroLava"], "LavaBubble", 100);
	AddAmbient("beaches", ["nearbyMat_Water", "nearbyMat_Sand", "inMat_Sky"], "ShoreWaves", 100);
	AddAmbient("nightowlsintrees", ["nearbyDefcall_IsTree", "IsNight_1"], "Owl", 10);
	AddAmbient("daybirdsintrees", ["nearbyDefcall_IsTree", "IsNight_0"], "Bird*", 20);
	AddAmbient("nightcicadasingrass", ["nearbyId_GRAS", "IsNight_1"], "Cicada*", 20);
	AddAmbient("underwater", ["inMat_Water"], "Underwater.ogg", 100);

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
		
		for (envnum = 0; envnum < GetLength(enviromentconditions); envnum++) {
			found = 1;

			for (var prop in enviromentconditions[envnum]) {
				var tmp = ExplodeString("_", prop);
				type = tmp[0];
				target = tmp[1];
				
				if (type == "IsNight") {
					found = found && Format("%d", IsNight()) == target;
				}
				
				if (type == "minPosY") {
					found = found && clonk->GetY() >= (StringToInt(target) * LandscapeHeight()) / 100;
				}
				if (type == "maxPosY") {
					found = found && clonk->GetY() <= (StringToInt(target) * LandscapeHeight()) / 100;
				}
				if (type == "minPosX") {
					found = found && clonk->GetX() >= (StringToInt(target) * LandscapeWidth()) / 100;
				}
				if (type == "maxPosX") {
					found = found && clonk->GetX() <= (StringToInt(target) * LandscapeWidth()) / 100;
				}
				
				if (type == "inMat") {
					found = found && GetMaterial(clonk->GetX(), clonk->GetY()) == Material(target);
				}
				
				if (type == "nearbyMat") {
					var haveMat = 0;
					for (var attempt = 0; attempt < 80; attempt++) {
						if (GetMaterial(clonk->GetX()+RandomX(-100, 100), clonk->GetY()+RandomX(-100, 100)) == Material(target)) {
							haveMat = 1;
							break;
						}
					}
					found = found && haveMat;
				}
				
				if (type == "nearbyDefcall") {
					var haveCall = 0;
					for (var obj in FindObjects(Find_Distance(30, clonk->GetX(), clonk->GetY()))) {
						if (DefinitionCall(GetID(obj), target)) {
							haveCall = 1;
							break;
						}
					}
					found = found && haveCall;
				}
				
				if (type == "nearbyId") {
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
	
	for (var plrid = 0; plrid <= lastplayerid; plrid++) {
		if (Local(plrid)) {
			for (var amb=0; amb <= GetLength(names); amb++) {
				if (Local(plrid)[0][amb] >= 3) {
					playSound(sounds[amb], amb, plrid, plrnum, volumes[amb], loops[amb]);
				}
				if (Local(plrid)[0][amb] <= 0) {
					stopSound(amb, plrid, plrnum);
				}
			}
			plrnum++;
		}
	}
}

private func playSound(sound, ambid, plrid, plrnum, volume, loop) {
	if (GetType(volume) == C4V_String && IndexOf("script_", volume) != -1) {
		volume = eval(Substring(volume, 7));
		if (volume == 0) volume = -1;
	}
	
	
	// Huge hax to allow e.g. looping wind sounds with freely changing volume - we need to alternate between 2 of the same sounds because it seems we cannot stop and start one sound in the same frame :(
	// To any engine coder: Please code volume adjustments of running sounds :<
	if (loop) {
		if (Local(plrid)[1][ambid] && Local(plrid)[1][ambid]->Local(0) != volume) {
			//Log("stop old sound %d != %d", Local(plrid)[1][ambid]->Local(0), volume);
			
			if (Local(plrid)[1][ambid]->Local(1) == ConcatStrings(sound, "1")) {
				sound = ConcatStrings(sound, "2");
			} else {
				sound = ConcatStrings(sound, "1");
			}
			
			stopSound(ambid, plrid, plrnum); 
			
			
		} else {
			if (Local(plrid)[1][ambid]) {
				sound = Local(plrid)[1][ambid]->Local(1); 
			} else {
				sound = ConcatStrings(sound, "1");
			}
		}
	}
	
	if (!Local(plrid)[1][ambid]) {
		Local(plrid)[1][ambid] = CreateObject(AMBH, 0, 0, -1);
		Local(plrid)[1][ambid]->Local(0) = volume;
		Local(plrid)[1][ambid]->Local(1) = sound;
	}
	
	
	if (Random(100) < playchances[ambid]) {
		if (volume == -1) {
			stopSound(ambid, plrid, plrnum); 
		} else {
			//Log("play sound %v with vol %v (ambid %d, plrid %d, plrnum %d)", sound, volume, ambid, plrid, plrnum);
			Local(plrid)[1][ambid]->Sound(sound, true, 0, volume, plrnum+1, loop);
		}
	}
}


private func stopSound(ambid, plrid, plrnum) {
	if (Local(plrid)[1][ambid]) {
		//Log("sound %v stopped for player %d (%v)", sounds[ambid], plrnum+1, Local(plrid)[1][ambid]);
		Local(plrid)[1][ambid]->Sound(Local(plrid)[1][ambid]->Local(1), true, 0, 100, plrnum+1, -1);
		RemoveObject(Local(plrid)[1][ambid]);
	}
}

