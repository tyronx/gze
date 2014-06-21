#strict

local playlists;
local playlistnames;
//local shuffle, loop;

local currentplaylistIndex;
local currentplaylist;

func Initialize() {
	playlists = [];
	playlistnames = [];
	//shuffle = 1;
	//loop = 1;
	AddDefaultPlaylists();
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
	
	CreatePlaylist("hell", [
		"567844_Hello-ZeppSaw-Theme.ogg",
		"568516_Dungeon-Deep-1.ogg",
		"Bezo - UWXXIX - Demon Lord (Fix) (ID 424274).ogg",
		"Brian Czernikowski (Cherni-Braiton) - unknown title.ogg"
	]);
	
	CreatePlaylist("peaceful-nightloop1", [
		"Night Loop 1.ogg"
	]);
	CreatePlaylist("peaceful-nightloop2", [
		"Night Loop 2.ogg"
	]);
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
func SetPlaylist(playlistname) {
	currentplaylist = playlistname;
	currentplaylistIndex = ArrayIndexOf(playlistnames, playlistname);
	
	if (currentplaylistIndex != -1) {
		SetPlayList(ImplodeArray(";", playlists[currentplaylistIndex]));
	}
}

func StopMusic() {
	Music();
}

func StartMusic() {
	SetPlayList(ImplodeArray(";", playlists[currentplaylistIndex]));
}


/*
func SetShuffle(mode) {
	shuffle = mode;
}

func SetLoop(mode) {
	loop = mode;
}
*/