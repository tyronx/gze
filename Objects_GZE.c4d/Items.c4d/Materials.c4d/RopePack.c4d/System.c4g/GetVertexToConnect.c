/*-- Seil-Ankn�pfungspunkte f�r verschiedene Objekte --*/

#strict 2

#appendto BLMP
#appendto BALN
#appendto TREE

public func GetVertexToConnect()
{
  // B�ume: An der Krone anknoten, au�er B�sche
  if(this()->~IsTree() && GetID() != BUSH) return 2;
  // Luftschiffe und Balone: in der Mitte anknoten
  if(GetID() == BLMP || GetID() == BALN) return -1;
}
