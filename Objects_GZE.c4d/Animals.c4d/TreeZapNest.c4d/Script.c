/*-- Zapnest --*/

#strict 2
#include ZAPN

protected func Initialize() {
	ScheduleCall(this(), "LaunchZap", RandomX(0,5000));
}

func LaunchZap() {
	if (!IsNight()) CreateObject(ZAP1);
	//zap->CollectHoneyTo(this());
	
	ScheduleCall(this(), "LaunchZap", RandomX(500,5000));
}

