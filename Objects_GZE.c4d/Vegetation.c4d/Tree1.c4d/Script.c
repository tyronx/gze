/*-- Baum --*/

#strict 2

#include TREE

func CanHouseZapNest() { return true; }
func CreateZapNestVertex() { return AddVertex(-8+Random(16),13); }
func TreeType() { return "deciduous"; }
public func ShakeStrength() {
  if (this->GetID()==TRE1){
     return 2; // not so shaky
  } else {
     return 3; // normal shakiness
  }
}