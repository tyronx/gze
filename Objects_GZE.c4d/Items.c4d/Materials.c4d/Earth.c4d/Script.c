/*--- Erde ---*/

#strict 2

protected func Hit()
{
  CastPXS("Earth", 200, 20);
  RemoveObject();
  return(1);
}

func IsAlchemContainer() { return(true); }
func AlchemProcessTime() { return(120); }