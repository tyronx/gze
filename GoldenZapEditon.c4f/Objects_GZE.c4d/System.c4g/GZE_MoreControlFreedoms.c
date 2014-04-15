#strict
#appendto KNIG

public func ControlThrow() {
	// Träger ermitteln
	var heldItem = Contents(0);
		
	// Träger bereit zum Werfen?
	if (heldItem && GetAction() eq "Jump") {
		if (SetAction("JumpThrow"))  {
			return(1);
		}
	}
	
	return(_inherited());
}

public func ControlLeft() {
	if (GetAction() eq "Jump") SetDir(DIR_Left());
	return(_inherited());
}

public func ControlRight() {
	if (GetAction() eq "Jump") SetDir(DIR_Right());
	return(_inherited());
}