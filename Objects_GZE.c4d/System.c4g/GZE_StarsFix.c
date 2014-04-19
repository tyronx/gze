/*-- Sterne sollen bei hellem Himmel nicht angezeigt werden --*/

#strict 2
#appendto STAR

public func IsLight(){ return true;} // das ist vllt ein ziemlich böser Hack?

public func Update()
{
	SetObjAlpha(Max( 0, 255 - 255 * GetDarknessGradeRel() / 100)); // damit sollten die Sterne bei Tag verschwinden
}
