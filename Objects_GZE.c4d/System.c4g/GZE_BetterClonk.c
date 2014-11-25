#strict 2
#appendto CLNK


/* Jump throws and turn around midair */

public func ControlThrow() 
{	
	if (GetPlrDownDouble(GetOwner())) return(0);	
	// Träger bereit zum Werfen?
	if (Contents(0) && GetAction() == "Jump") 
		if (SetAction("JumpThrow"))  { return(1); }
	return(_inherited());
}

public func ControlLeft() 
{
	if (GetAction() == "Jump") SetDir(DIR_Left);
	return(_inherited());
}

public func ControlRight() 
{
	if (GetAction() == "Jump") SetDir(DIR_Right);
	return(_inherited());
}