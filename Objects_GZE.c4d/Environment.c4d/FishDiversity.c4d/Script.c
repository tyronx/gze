/*-- Fish diversity --*/

#strict 2

local intialized;

local fishColor;
local fishSize;
local schooling;

protected func Initialize()  {
	ScheduleCall(this, "LateInitialize", 1);
}

func LateInitialize() {
	var fishCount = ObjectCount2(Find_OCF(OCF_Living), Find_Func("IsFish"));
	var types = Sqrt(fishCount);
	if (types > 10) types += RandomX(-5, 5);
	else if (types > 3) types += RandomX(-2, 3);
	
	fishColor = [];
	fishSize = [];
	schooling = [];
	
	while (types-- > 0) {
		ArrayPush(fishColor, HSL(Random(256),255,100+Random(60)));
		ArrayPush(fishSize, Min(100, RandomX(40, 110)));
		ArrayPush(schooling, Random(2));
	}
	
	var types = GetLength(fishSize);
	var i = 0, type;
	for (var pFish in FindObjects(Find_OCF(OCF_Living), Find_Func("IsFish"))) {
		type = (types * i) / fishCount;
		pFish->SetColorDw(fishColor[type]);
		pFish->SetCon(fishSize[type]);
		if (schooling[type]) {
			pFish->AddActivity("Schooling", 3);
		}
		Local(0, pFish) = type;
		i++;
	}
	
	ClearScheduleCall(this, "LateInitialize");
	intialized = 1;
}


func Colorize(pFish) {
	if (intialized) {
		var type = Random(GetLength(fishSize));
		pFish->SetColorDw(fishColor[type]);
		pFish->SetCon(fishSize[type]);
		if (schooling[type]) {
			pFish->AddActivity("Schooling", 2);
		}
		Local(0, pFish) = type;
	}
}