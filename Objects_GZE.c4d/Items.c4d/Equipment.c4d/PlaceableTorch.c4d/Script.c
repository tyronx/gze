/*-- Fackel --*/

#strict 2

// Called by Anvil when the object is being created
public func Produced() {
	CreateObject(GetID());
}

public func Activate() {
	[$TxtPlaceTorch$]
	
	if (GetMaterial() != Material("Tunnel")) {
		Sound("CommandFailure*");
		return Message("$TxtNoBackground$", this);
	}
	
	if (FindObject2(Find_ID(TORC), Find_Distance(100))) {
		Sound("CommandFailure*");
		return Message("$TxtTooClose$", this);
	}
	
	Sound("WoodHit*");
	CreateObject(TORC, 0, 10, -1)->Place();
	RemoveObject();
	
	return 1;
}
  
func Hit() {
	Sound("WoodHit*");
}

func IsAnvilProduct() { return 2; }