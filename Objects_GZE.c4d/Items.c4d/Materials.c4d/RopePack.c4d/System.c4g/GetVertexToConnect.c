/*-- Seil-Anknüpfungspunkte für verschiedene Objekte --*/

#strict 2

#appendto BLMP
#appendto BALN
#appendto TREE

public func GetVertexToConnect()
{
  // Bäume: An der Krone anknoten, außer Büsche
  if(this()->~IsTree() && GetID() != BUSH) return 2;
  // Luftschiffe und Balone: in der Mitte anknoten
  if(GetID() == BLMP || GetID() == BALN) return -1;
}
