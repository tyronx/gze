/*-- In our world, Monsters live in caves, Fire monsters live at low level caves --*/

#strict 2
#appendto MONS
#appendto FMNS

func GetAnimalPlacementLineTop() { if (GetID()==FMNS) return(94); else return(50);  }
func GetAnimalPlacementLineBottom() { if (GetID()==FMNS) return(100); else return(80);  }
func GetAnimalPlacement() { return(3); }
