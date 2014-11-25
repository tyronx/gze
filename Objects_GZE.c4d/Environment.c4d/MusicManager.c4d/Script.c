#strict 2

local statiByPlayer;
local maxPlayerIndex;

local names, enviromentconditions, volumes, looplists, loopsingles;
local playlists;
local playlistnames;
//local currentplaylistIndex;
//local currentplaylist;


// Local variable usage:

// Local(0..highest playerid)   - Local index coresponds to player id
// = Array ( 
//		0 = Array([amount of env conditions numbers: between 0 and 3, whereas 0 means stop that sound and 3 means play that music
//		1 = index of current playing music or -1
//	)


func Initialize() {
	names = [];
	enviromentconditions = [];
	
	volumes = [];
	looplists = [];
	loopsingles = [];
	playlists = [];
	playlistnames = [];
	
	AddDefaultPlaylists();
	
	if (GameCall("MusicTheme")) {
		SetPlaylist(GameCall("MusicTheme"));
		RemoveObject();
		return;
	}
	
	// First trigger to be added = lowest priority
	// Last added trigger = highest priority
	// Music with higher priority stops music with lower priority
	AddTrigger("peaceful", ["IsNight_0"], 100, 0, 0);
	AddTrigger("peacefulnightloops", ["IsNight_1"], 100, 0, 1);
	AddTrigger("hell", ["nearbyMat_DuroLava", "minPosY_85"], 100, 0, 0);
	
	
	
	AddEffect("Detect", this, 1, 40, this);
}

func FxDetectTimer() {
	//Log("%v", Local(0));
	checkPlayerCount();
	updatePlayerStati();
	playApplicableMusic();
}

func AddDefaultPlaylists() {
	CreatePlaylist("peaceful", [
		"484019_Blind-Alley.ogg",
		"515887_Red.ogg",
		"550015_Rebuild.ogg",
		"559676_In-a-While.ogg",
		"560546_Homecoming.ogg",
		"561630_Hey-You.ogg",
		"566414_AGP3-03Still-so-man.ogg",
		"576319_Disappointing-Love-.ogg",
		"Matt-Porter - Waiting (Clean) (ID 465344).ogg",
		"Grandvision - Soaring Soren.ogg",
		"Semaphore - Uprising Good (ID 438371).ogg",
		"Sirtroymoon - A Moonlight Waltz (ID 126585).ogg",
		"gamejunkie - Child in my arms (ID 432848).ogg",
		"Steve Syz - #4 mp3.ogg"
	]);
	
	CreatePlaylist("peacefulnightloops", [
		"Night Loop 1.ogg",
		"Night Loop 2.ogg"
	]);
	
	CreatePlaylist("hell", [
		"567844_Hello-ZeppSaw-Theme.ogg",
		"568516_Dungeon-Deep-1.ogg",
		"Bezo - UWXXIX - Demon Lord (Fix) (ID 424274).ogg",
		"Brian Czernikowski (Cherni-Braiton) - unknown title.ogg"
	]);

}

// param looplist does nothing currently!
func AddTrigger(string name, array conditions, int volume, int looplist, int loopsingle) {
	ArrayPush(names, name);
	ArrayPush(enviromentconditions, conditions);
	
	
	ArrayPush(volumes, volume);
	ArrayPush(looplists, looplist);
	ArrayPush(loopsingles, loopsingle);
}

func RemoveTrigger(string name) {
	var index = ArrayIndexOf(names, name);
	if (index > -1) {
		ArraySlice(names, index);
		ArraySlice(enviromentconditions, index);

		ArraySlice(volumes, index);
		ArraySlice(looplists, index);
		ArraySlice(loopsingles, index);
	}
}

// Plays a playlist, overridding completely all triggers
func SetPlaylist(name) {
	var index = ArrayIndexOf(playlistnames, name);
	var playlist = playlists[index];
	SetPlayList(ImplodeArray(";", playlist));
}

func CreatePlaylist(playlistname, songs) {
	if (!songs) songs = [];
	
	ArrayPush(playlistnames, playlistname);
	ArrayPush(playlists, songs);
}

func AddSongToPlaylist(playlistname, song) {
	var index = ArrayIndexOf(playlistnames, playlistname);
	
	ArrayPush(playlists[index], playlistname);
}

func RemoveSongFromPlaylist(playlistname, song) {
	var index = ArrayIndexOf(playlistnames, playlistname);
	
	var songindex = ArrayIndexOf(playlists[index], song);
	
	ArraySlice(playlists[index], songindex);
}

// If currently running a song of another playlist, this func will fade to a song of the other playlist
/*func SetPlaylist(playlistname) {
	currentplaylist = playlistname;
	currentplaylistIndex = ArrayIndexOf(playlistnames, playlistname);
	
	if (currentplaylistIndex != -1) {
		SetPlayList(ImplodeArray(";", playlists[currentplaylistIndex]));
	}
}*/





func checkPlayerCount() {
	var playerid;
	for (var i = 0; i < GetPlayerCount(C4PT_User); i++) {
		playerid= GetPlayerByIndex(i, C4PT_User);
		if (!Local(playerid)) {
			Local(playerid) = CreateArray(2);
			Local(playerid)[0] = CreateArray(GetLength(enviromentconditions));
			Local(playerid)[1] = -1;
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
		//	Log("checking %d", envnum);
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
			//	Log("%v %v %v", prop, target, found);
				
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


private func playApplicableMusic() {
	var lastplayerid = GetPlayerByIndex(GetPlayerCount(C4PT_User)-1, C4PT_User);
	var plrnum = 0;
	
	for (var plrid = 0; plrid <= lastplayerid; plrid++) {
		if (Local(plrid)) {
			for (var amb=0; amb <= GetLength(names); amb++) {
				if (Local(plrid)[0][amb] <= 0) {
					stopMusic(amb, plrid, plrnum);
				}
				//if (Local(plrid)[0][amb] >= 3 ) Log("%d > %d", amb, Local(plrid)[1]);
				if (Local(plrid)[0][amb] >= 3 && amb > Local(plrid)[1]) {
					startMusic(amb, names[amb], plrid, plrnum, volumes[amb], looplists[amb], loopsingles[amb]);
				}
			}
			plrnum++;
		}
	}
}





func stopMusic(listindex, plrid, plrnum) {
	// Only stop this music if its actually currently playing
	if (Local(plrid)[1] == listindex) {
		stopMusicForPlayer(plrnum);
		Local(plrid)[1] = -1;
	}
}

func startMusic(listindex, name, plrid, plrnum, volume, looplist, loopsingle) {
	if (listindex > Local(plrid)[1]) {
		stopMusicForPlayer(plrnum);
	} else {
		return;
	}

	MusicLevel(volume);
	
	var index = ArrayIndexOf(playlistnames, name);
	var playlist = playlists[index];
	Log("playlist is %v", playlist);
	
	if (loopsingle) {
		var single = playlist[RandomX(0, GetLength(playlist))];
		Log("single song of that is %v", single);
		setPlayListForPlayer(plrnum, single);
	} else {
		setPlayListForPlayer(plrnum, ImplodeArray(";", playlist));
	}
	Local(plrid)[1] = listindex;
}



func stopMusicForPlayer(plrnum) {
	if (GetPortrait(GetCursor(plrnum))) {
		//Log("stopped music");
		//Music();
	}
}
func setPlayListForPlayer(plrnum, playlist) {
	if (GetPortrait(GetCursor(plrnum))) {
		Log("playing list %v", playlist);
		SetPlayList(playlist);
	}
}



/*
func SetShuffle(mode) {
	shuffle = mode;
}

func SetLoop(mode) {
	loop = mode;
}
*/