/* A.I. Task based Control */

// TODO:
// - AIBase improvements: Activity Callbacks on Contact and ContainedLeft/Right/etc.

#strict 2

local activity;
local activities;

/* 
 * For each activity, write 2 functions. ShouldExecute[activity] and ContinueExecute[activity]
 *
 * ShouldExecuteXX can be used to initialize the activity. A return value of 1 will start this activity
 * ContinueExecuteXX can be used for controlling that activity. A return value of 0 will stop this activity
 * (optional) FinishExecuteXX is called when an activity ends
 *
 */


/* Set your activities here, sorted top-down by priority of execution */
func ActivityInit() {
	
}

func AddActivity(name, priority) {
	ArrayInsert(activities, name, priority);
}
func AddActivities(names) {
	for (name in names) {
		ArrayPush(activities, name);
	}
}

func RemoveActivity(name) {
	ArraySlice(activities, ArrayIndexOf(activities, name));
	if (activity == name) activity = 0;
}


func Initialize() {
	activities = [];
	ActivityInit();
	
	AddEffect("Activity", this, 1, 35, this);
	return;
}

func FxActivityStart() {
	// sets first activity
	FxActivityTimer();
	// calls ContinueExecute (otherwise there is a 1 second pause in animal behavior)
	FxActivityTimer();
}

// Called approx every 1 second
func FxActivityTimer() {
	if (activity) {
		if (!ObjectCall(this, Format("ContinueExecute%s", activity))) {
			ObjectCall(this, Format("FinishExecute%s", activity));
			activity = 0;
		}
	}
	
	
	for (strActivity in activities) {
		//Activities are sorted by priority. So if a higher priority activity is running, do not check on others
		if (activity == strActivity) break;
		
		// should execute a higher priority task? Then replace with current activity
		if (ObjectCall(this, Format("ShouldExecute%s", strActivity))) {
			ObjectCall(this, Format("FinishExecute%s", activity));
			activity = strActivity;
			
			break;
		}
	}
	return;
}
  

func ContactLeft() {
	Contact(COMD_Left);
}
func ContactBottom() {
	Contact(COMD_Down);
}
func ContactTop() {
	Contact(COMD_Up);
}
func ContactRight() {
	Contact(COMD_Right);
}

func Contact() {
}