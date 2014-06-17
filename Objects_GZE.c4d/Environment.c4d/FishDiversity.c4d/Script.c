/*-- Fish diversity --*/

#strict

local fishColor;
local fishSize;

protected func Initialize()  {
	ScheduleCall(this(), "LateInitialize", 1);
}

func LateInitialize() {
	var fishCount = ObjectCount2(Find_Func("IsFish"));
	var populations = Sqrt(fishCount);
	if (populations > 10) populations += RandomX(-5, 5);
	else if (populations > 3) populations += RandomX(-2, 3);
	
	fishColor = [];
	fishSize = [];
	
	while (populations-- > 0) {
		ArrayPush(fishColor, HSL(Random(256),255,100+Random(60)));
		ArrayPush(fishSize, Min(100, RandomX(40, 120)));
	}
	
	//for (var pFish in FindObjects2
	
  // Alle Fische anfangs umfärben
  var pFish;
  while(pFish=FindObject(FISH,0,0,0,0,0,0,0,0,pFish))
    Colorize(pFish);	
}

public func Colorize(pObj)  {
	SetColorDw(HSL(Random(256),255,100+Random(60)),pObj);
}
