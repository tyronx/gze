/*-- Funke --*/

#strict

protected func Process()
{
  if (40 < GetActTime()) RemoveObject();
  return(1);
}


protected func Completion()
{
  SetAction("Sparkle");
  return(1);
}