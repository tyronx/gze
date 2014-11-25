/*-- Baum --*/

#strict 2

#include TREE

func CanHouseZapNest() { return true; }
func CreateZapNestVertex() { return AddVertex(-8+Random(16),13); }

func CanBeChoppedInHalf() { return true; }
func CanCastLeaves(){ return true; }