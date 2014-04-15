#strict
#appendto TRE1
#appendto TRE2
#appendto TRE3

local zapNest;

public func Construction() {
	ScheduleCall(this(), "PutZapnest", 2);

	return(inherited());
}

public func PutZapnest() {
	if (GetCon() != 100 || GetR() != 0) return(0);
	
	if (!Random(10) && GetCon()>50 && !FindObject2(Find_ID(ZAPT), Find_InRect(-40,-40,80,80))) {
		zapNest = CreateObject(ZAPT);
		
		if (GetID() == TRE1) {
			AddVertex(-8+Random(16),13);
		}
		if (GetID() == TRE2) {
			AddVertex(-8+Random(16),2);
		}

		zapNest->SetAction("HangOnTree", this());
		zapNest->SetActionData(4);
	}
}

public func ChopDown() {
	if (zapNest) {
		zapNest->SetAction("Idle");
	}
	
	return(inherited());
}
