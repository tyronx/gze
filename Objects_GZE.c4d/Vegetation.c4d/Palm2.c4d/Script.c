/*-- Palme --*/

#strict 2

#include TRE1

// Etwas leichter zu f�llen
private func TreeStrength() { return 100; }
public func GetVegetationSoil() { return "Sand"; }
public func GetVegetationRotates() { return [-45, 45]; }

/* �berladungen f�r zus�tzliche Sturm-Animation */  

private func Breeze() {
  if (Abs(GetWind()) < 50 + MotionThreshold) SetAction("Still");
  if (Abs(GetWind()) > 74 + MotionThreshold) SetAction("Storm");
}
    
private func Storm() {
  if (Abs(GetWind()) < 75 + MotionThreshold) SetAction("Breeze");
}