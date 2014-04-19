/*-- Fackel --*/

#strict 2

public func Activate() {
	if (GetMaterial() == Material("Tunnel")) {
		CreateObject(TORC,0,8)->Activate();
		RemoveObject();
	}
  
}
  
func Hit() {
	Sound("WoodHit*");
}

func IsAnvilProduct() { return 1; }