/*-- Gefangennahme --*/

#strict 2

protected func Activate(iByPlayer)
{
  MessageWindow(GetDesc(), iByPlayer);
  return 1;
}
